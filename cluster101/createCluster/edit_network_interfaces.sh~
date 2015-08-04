#!/bin/bash

# IP address variable
i=2

# Create temporary directory for mounting image files
sudo mkdir /mnt/tmp/

for image_file in *.img;
do
	# Mount current image file
	sudo mount -o loop,offset=$((512*2048)) $image_file /mnt/tmp

	# Upgrade privileges of /mnt/tmp/etc/network/interfaces file
	sudo chmod 646 /mnt/tmp/etc/network/interfaces

	# Edit /mnt/tmp/etc/network/interfaces file

	sudo echo "auto lo" > /mnt/tmp/etc/network/interfaces
	sudo echo "iface lo inet loopback" >> /mnt/tmp/etc/network/interfaces
	sudo echo "auto eth0" >> /mnt/tmp/etc/network/interfaces
	sudo echo "iface eth0 inet static" >> /mnt/tmp/etc/network/interfaces
	sudo echo "address 192.168.122.$i" >> /mnt/tmp/etc/network/interfaces
	sudo echo "netmask 255.255.255.0" >> /mnt/tmp/etc/network/interfaces
	sudo echo "gateway 192.168.122.1" >> /mnt/tmp/etc/network/interfaces
	sudo echo "broadcast 192.168.122.255" >> /mnt/tmp/etc/network/interfaces

	# Downgrade privileges of /mnt/tmp/etc/network/interfaces file
	sudo chmod 644 /mnt/tmp/etc/network/interfaces

	# Unmount current image file
	sudo umount /mnt/tmp

	# Increment IP address variable
	((i = i + 1))
done
