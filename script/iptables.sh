iptables -t nat -A PREROUTING -d 127.0.0.1/32 -j RETURN
iptables -t nat -A PREROUTING -d 224.0.0.0/4 -j RETURN
iptables -t nat -A PREROUTING -d 255.255.255.255/32 -j RETURN
iptables -t nat -A PREROUTING -d 192.168.0.0/16 -j RETURN
iptables -t nat -A PREROUTING -m mark --mark 0xff -j RETURN
iptables -t nat -A PREROUTING -p tcp -j DNAT --to-destination 127.0.0.1:12345

iptables -t nat -A OUTPUT -d 127.0.0.1/32 -j RETURN
iptables -t nat -A OUTPUT -d 224.0.0.0/4 -j RETURN
iptables -t nat -A OUTPUT -d 255.255.255.255/32 -j RETURN
iptables -t nat -A OUTPUT -d 192.168.0.0/16 -j RETURN
iptables -t nat -A OUTPUT -m mark --mark 0xff -j RETURN
iptables -t nat -A OUTPUT -p tcp -j DNAT --to-destination 127.0.0.1:12345