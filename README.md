
# Set static IP

1. Get IP address
```hostname -I```

2. Get router's IP
```ip r | grep default```

3. Check network connection name
```nmcli connection show```

4. See current config
```nmcli connection show "<name>" | grep ipv4```

5. Set static IP
``` sudo nmcli connection modify "<name>" 
ipv4.addresses 192.168.xx.xxx/24 
ipv4.gateway 192.168.xx.x 
ipv4.dns 192.168.29.1 
ipv4.method manual```

6. Change name
``` sudo nmcli connection modify "<old_name>" connection.id "<new_name>"```

7. Restart
``` sudo nmcli connection up "<name>"

