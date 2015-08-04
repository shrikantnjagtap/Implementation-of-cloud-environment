HOSTIP=`ifconfig eth0  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`
sudo vconfig add eth0 701
sudo brctl addbr br1_1
sudo brctl addif br1_1 eth0.701
sudo ifconfig br1_1 192.168.1.1 netmask 255.255.255.0 up
sudo ifconfig eth0.701 up 
sudo iptables -t filter -A INPUT -i eth0.701 -p udp -m udp --dport 53 -j ACCEPT
sudo iptables -t filter -A INPUT -i eth0.701 -p tcp -m tcp --dport 53 -j ACCEPT
sudo iptables -t filter -A INPUT -i eth0.701 -p udp -m udp --dport 67 -j ACCEPT
sudo iptables -t filter -A INPUT -i eth0.701 -p tcp -m tcp --dport 67 -j ACCEPT
sudo iptables -t filter -A FORWARD -d 192.168.1.0/24 -o eth0.701 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -t filter -A FORWARD -s 192.168.1.0/24 -i eth0.701 -j ACCEPT
sudo iptables -t filter -A FORWARD -i eth0.701 -o eth0.701 -j ACCEPT
sudo iptables -t filter -A FORWARD -o eth0.701 -j REJECT --reject-with icmp-port-unreachable
sudo iptables -t filter -A FORWARD -i eth0.701 -j REJECT --reject-with icmp-port-unreachable

sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 ! -d 192.168.1.0/24 -p tcp -j MASQUERADE --to-ports 1024-65535
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 ! -d 192.168.1.0/24 -p udp -j MASQUERADE --to-ports 1024-65535
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 ! -d 192.168.1.0/24 -j MASQUERADE
sudo iptables -t mangle -A POSTROUTING -o eth0.701 -p udp -m udp --dport 68 -j CHECKSUM --checksum-fill

sudo iptables -A  INPUT -i eth0 -p tcp --dport 10001 -m state --state NEW,ESTABLISHED -j ACCEPT
sudo iptables -A  OUTPUT -o eth0 -p tcp --sport 10001 -m state --state ESTABLISHED -j ACCEPT
sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport 10001 -j DNAT --to 192.168.1.2:22
sudo iptables -A  INPUT -i eth0 -p tcp --dport 50001 -m state --state NEW,ESTABLISHED -j ACCEPT
sudo iptables -A  OUTPUT -o eth0 -p tcp --sport 50001 -m state --state ESTABLISHED -j ACCEPT
sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport 50001 -j DNAT --to 192.168.1.2:80
