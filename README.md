# SETUP
# Set static IP

1. Get IP address
```
hostname -I
```

2. Get router's IP
```
ip r | grep default
```

3. Check network connection name
```
nmcli connection show
```

4. See current config
```
nmcli connection show "<name>" | grep ipv4
```

5. Set static IP
```
sudo nmcli connection modify "<name>" 
ipv4.addresses 192.168.xx.xxx/24 
ipv4.gateway 192.168.xx.x 
ipv4.dns 192.168.29.1 
ipv4.method manual
```

6. Change name
```
sudo nmcli connection modify "<old_name>" onnection.id "<new_name>"
```

7. Restart
```
sudo nmcli connection up "<name>"
```

8. Enable Camera in `/boot/firmware/config.txt`
```
dtoverlay=imx219
```

9. Build
```
gcc -Wall -pthread -o main main.c -lpigpiod_if2 -lcurl -lrt
```

# View Stream
```
http://<IP>:8889/cam
```

# View Recordings
```
http://<IP>:9996/list?path=cam
```

'''
sudo apt install libcurl4-openssl-dev
'''


create service:
```
sudo nano /etc/systemd/system/birdcam.service
```

```
[Unit]
Description=Bird Cam stack (pigpiod + mediamtx + main)
After=network.target

[Service]
Type=simple
User=root
ExecStartPre=/bin/bash -c 'pidof pigpiod >/dev/null || pigpiod'
ExecStart=/bin/bash -c '/home/isopod/mediamtx /home/isopod/Bird-Cam/mediamtx.yml & sleep 1; exec /home/isopod/Bird-Cam/code/main'
ExecStop=/bin/bash -c 'killall -INT main; killall mediamtx; killall pigpiod || true'
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```



Add the following to cron

```
crontab -e
```

```
# Start at 8am
0 8 * * * /bin/systemctl start birdcam

# Stop at 5pm
0 17 * * * /bin/systemctl stop birdcam

# On boot, start if between 8–17
@reboot sleep 10 && /home/pi/Bird-Cam/start-if-hours.sh
```