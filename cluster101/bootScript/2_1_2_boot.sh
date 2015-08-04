HOSTIP=`ifconfig eth0  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`
sudo iptables -A  INPUT -i eth0 -p tcp --dport 10258 -m state --state NEW,ESTABLISHED -j ACCEPT
sudo iptables -A  OUTPUT -o eth0 -p tcp --sport 10258 -m state --state ESTABLISHED -j ACCEPT
sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport 10258 -j DNAT --to 192.168.17.3:22
sudo iptables -A  INPUT -i eth0 -p tcp --dport 50258 -m state --state NEW,ESTABLISHED -j ACCEPT
sudo iptables -A  OUTPUT -o eth0 -p tcp --sport 50258 -m state --state ESTABLISHED -j ACCEPT
sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport 50258 -j DNAT --to 192.168.17.3:80
