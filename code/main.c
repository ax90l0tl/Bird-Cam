#include <stdio.h>
#include <pigpiod_if2.h>

#define SENSOR_PIN  25

int main() {
int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
fprintf(stderr, "pigpio init failed\n");
return 1;
}
set_mode(pi, SENSOR_PIN, PI_INPUT);
set_pull_up_down(pi, SENSOR_PIN, PI_PUD_DOWN);

while(1){
fprintf(stdout, "output: %d\n", gpio_read(pi, SENSOR_PIN));
}
pigpio_stop(pi);
}

