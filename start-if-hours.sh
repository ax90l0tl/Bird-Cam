#!/bin/bash
hour=$(date +%H)
if [ "$hour" -ge 8 ] && [ "$hour" -lt 17 ]; then
  systemctl start birdcam
fi
