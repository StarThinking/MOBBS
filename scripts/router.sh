echo "1" > /proc/sys/net/ipv4/ip_forward

iptables -t nat -F
iptables -t nat -A PREROUTING -d 192.168.3.7 -i eth3 -j DNAT --to-destination 10.0.0.7
iptables -t nat -A POSTROUTING -s 10.0.0.0/255.255.255.0 -o eth1 -j SNAT --to-source 192.168.3.7

