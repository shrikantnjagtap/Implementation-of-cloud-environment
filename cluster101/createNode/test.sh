#!/bin/bash


# IP address variable

i=2

for image_file in *.sh;
do
	echo "$image_file : address 192.168.122.$i"
	((i = i + 1))
done

