#include <stdio.h>
#include <pigpiod_if2.h>
#include <curl/curl.h>
#include <signal.h>
#include <math.h>
#include <time.h>

#define SENSOR_PIN 23
#define API_ENDPOINT "http://localhost:9997/v3/config/paths/patch/cam"

static volatile int keepRunning = 1;
uint32_t last_motion_tick = 0;
time_t now;
struct tm *tm;
void start_recording(uint8_t state)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    const char *body = state ? "{\"record\": true}" : "{\"record\": false}";
    curl_easy_setopt(curl, CURLOPT_URL, API_ENDPOINT);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    printf("Recording %s\n", state ? "started" : "stopped");
}

int verify_route()
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return 0;

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9997/v3/config/global/get");
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}
void detected_motion(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
    printf("GPIO %u level=%u tick=%.2f\n", user_gpio, level, pow(10, -6) * (float)(tick - last_motion_tick));
    last_motion_tick = tick;
    start_recording(1);
}

void handle_sigint(int sig)
{
    keepRunning = 0;
}

int main()
{
    signal(SIGINT, handle_sigint);
    curl_global_init(CURL_GLOBAL_ALL);
    int pi = pigpio_start(NULL, NULL);
    printf("%i\n", pi);
    if (pi < 0)
    {
        fprintf(stderr, "pigpio init failed\n");
        return 1;
    }
    printf("%i\n", set_mode(pi, SENSOR_PIN, PI_INPUT));
    printf("%i\n", set_pull_up_down(pi, SENSOR_PIN, PI_PUD_DOWN));
    printf("Calibrating sensor...\n");
    time_sleep(15.0);

    if (!verify_route())
    {
        fprintf(stderr, "Can't reach Mediamtx server\n");
        return 1;
    }

    int cb_id = callback(pi, SENSOR_PIN, RISING_EDGE, detected_motion);

    if (cb_id < 0)
    {
        fprintf(stderr, "callback failed: %d\n", cb_id);
        pigpio_stop(pi);
        return 1;
    }
    // start_recording(1);
    while (keepRunning)
    {
        // printf("GPIO %u level=%u\n", SENSOR_PIN, gpio_read(pi, SENSOR_PIN));
        time(&now);
        tm = localtime(&now);
        printf("Current time: %02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
        if (tm->tm_hour >= 17)
        {
            keepRunning = 0;
            break;
        }
        time_sleep(60.0);
    }
    callback_cancel(cb_id);
    printf("Stopping recording...\n");
    start_recording(0);
    curl_global_cleanup();
    pigpio_stop(pi);
    return 0;
}
