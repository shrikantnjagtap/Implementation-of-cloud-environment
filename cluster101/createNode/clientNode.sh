#!/bin/bash

HOSTIP="172.17.12.120"

#   first arg image file $1 =1.img subnet address=cluster_id=$2 ===> 192.168.<cluster_id>.<nide_id+1> node_id=$3



# Create temporary directory for mounting image files
sudo mkdir /mnt/tmp/
	
	file="/home/cluster101/$1/$2/$3.img"
	# Mount current image file
	sudo mount -o loop,offset=$((512*2048)) $file /mnt/tmp

	# Upgrade privileges of /mnt/tmp/etc/network/interfaces file
	#sudo chmod 646 /mnt/tmp/etc/network/interfaces
	#sudo chmod 646 /mnt/tmp/etc/hosts
	#sudo chmod 646 /mnt/tmp/etc/fstab
	
	# Edit /mnt/tmp/etc/network/interfaces file

	i="$(($3+1))"
	ip="$((($1-1)*16+$2))"                  #192.168.<>.2--254
	
	sudo echo "auto lo" > /mnt/tmp/etc/network/interfaces
	sudo echo "iface lo inet loopback" >> /mnt/tmp/etc/network/interfaces
	sudo echo "auto eth0" >> /mnt/tmp/etc/network/interfaces
	sudo echo "iface eth0 inet static" >> /mnt/tmp/etc/network/interfaces
	sudo echo "address 192.168.$ip.$i" >> /mnt/tmp/etc/network/interfaces
	sudo echo "netmask 255.255.255.0" >> /mnt/tmp/etc/network/interfaces
	sudo echo "gateway 192.168.$ip.1" >> /mnt/tmp/etc/network/interfaces
	sudo echo "broadcast 192.168.$ip.255" >> /mnt/tmp/etc/network/interfaces
	#add name server
	sudo echo "dns-nameservers 8.8.8.8" >> /mnt/tmp/etc/network/interfaces
	
	
	# write all the addresses
	sudo echo "127.0.0.1       localhost" > /mnt/tmp/etc/hosts
	sudo echo "127.0.1.1       ubuntu" >> /mnt/tmp/etc/hosts   
	sudo echo "192.168.$ip.2    node1" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.3    node2" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.4    node3" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.5    node4" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.6    node5" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.7    node6" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.8    node7" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.9    node8" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.10   node9" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.11   node10" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.12   node11" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.13   node12" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.14   node13" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.15   node14" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.16   node15" >> /mnt/tmp/etc/hosts
	sudo echo "192.168.$ip.17   node16" >> /mnt/tmp/etc/hosts

	sudo echo "# The following lines are desirable for IPv6 capable hosts" >> /mnt/tmp/etc/hosts
	sudo echo "::1     ip6-localhost ip6-loopback" >> /mnt/tmp/etc/hosts	
	sudo echo "fe00::0 ip6-localnet" >> /mnt/tmp/etc/hosts
	sudo echo "ff00::0 ip6-mcastprefix" >> /mnt/tmp/etc/hosts
	sudo echo "ff02::1 ip6-allnodes" >> /mnt/tmp/etc/hosts
	sudo echo "ff02::2 ip6-allrouters" >> /mnt/tmp/etc/hosts

	
	
	
		
	#add rule to UFW
	sudo echo "### tuple ### allow any any 0.0.0.0/0 any 192.168.$ip.0/24 in" >> /mnt/tmp/lib/ufw/user.rules
	sudo echo "-A ufw-user-input -s 192.168.$ip.0/24 -j ACCEPT" >> /mnt/tmp/lib/ufw/user.rules
	sudo echo "### END RULES ###" >> /mnt/tmp/lib/ufw/user.rules
		
	#write to /etc/fstab to share [ONLY IN COMPUTE NODES]
	sudo echo "node1:/home/deben /home/deben nfs" >> /mnt/tmp/etc/fstab

	
	# Downgrade privileges of /mnt/tmp/etc/network/interfaces file
	#sudo chmod 644 /mnt/tmp/etc/network/interfaces
	#sudo chmod 644 /mnt/tmp/etc/hosts
	#sudo chmod 644 /mnt/tmp/etc/fstab

	
	#add iptable config in host for ssh starts from 10000
	port="$((10000+(($1-1)*16)+(($2-1)*16)+$3))"
	sudo iptables -A  INPUT -i eth0 -p tcp --dport $port -m state --state NEW,ESTABLISHED -j ACCEPT
	sudo echo "sudo iptables -A  INPUT -i eth0 -p tcp --dport $port -m state --state NEW,ESTABLISHED -j ACCEPT" > /home/cluster101/bootScript/$1_$2_$3_boot.sh
	sudo iptables -A  OUTPUT -o eth0 -p tcp --sport $port -m state --state ESTABLISHED -j ACCEPT
	sudo echo "sudo iptables -A  OUTPUT -o eth0 -p tcp --sport $port -m state --state ESTABLISHED -j ACCEPT" >> /home/cluster101/bootScript/$1_$2_$3_boot.sh
	sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport $port -j DNAT --to 192.168.$ip.$i:22
	sudo echo "sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport $port -j DNAT --to 192.168.$ip.$i:22" >> /home/cluster101/bootScript/$1_$2_$3_boot.sh
		
	#add iptable config in host for collectd starts from 50000
	cport="$((50000+(($1-1)*16)+(($2-1)*16)+$3))"
	sudo iptables -A  INPUT -i eth0 -p tcp --dport $cport -m state --state NEW,ESTABLISHED -j ACCEPT
	sudo echo "sudo iptables -A  INPUT -i eth0 -p tcp --dport $cport -m state --state NEW,ESTABLISHED -j ACCEPT" >> /home/cluster101/bootScript/$1_$2_$3_boot.sh
	sudo iptables -A  OUTPUT -o eth0 -p tcp --sport $cport -m state --state ESTABLISHED -j ACCEPT
	sudo echo "sudo iptables -A  OUTPUT -o eth0 -p tcp --sport $cport -m state --state ESTABLISHED -j ACCEPT" >> /home/cluster101/bootScript/$1_$2_$3_boot.sh
	sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport $cport -j DNAT --to 192.168.$ip.$i:80
	sudo echo "sudo iptables -t nat -A PREROUTING -p tcp -d $HOSTIP --dport $cport -j DNAT --to 192.168.$ip.$i:80" >> /home/cluster101/bootScript/$1_$2_$3_boot.sh
	
	# Unmount current image file
	sudo umount /mnt/tmp/
