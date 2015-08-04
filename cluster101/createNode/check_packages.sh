#!/bin/bash

# Check whether the packages in the list are installed or not

list=("openssh-client" "openssh-server")

for pkg in ${list[@]}
do
	if sudo apt-get -qq install $pkg; then
		echo "Successfully installed : $pkg"
	else
		echo "Error installing : $pkg"
	fi
done

