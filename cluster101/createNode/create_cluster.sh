#!/bin/bash

cluster_base_directory="/home/cluster101"
cluster_name=$1

if [ "${#cluster_name}" -lt 1 ]; then
	echo ""
	echo "ERROR: Please provide valid CLUSTER_NAME as an ARGUMENT to this COMMAND"
	echo ""
	echo "COMMAND USAGE: bash create_cluster.sh CLUSTER_NAME"
	echo ""
	exit 1
else
	sudo ./createCluster $cluster_name
	sudo cp edit_network_interfaces.sh $cluster_base_directory/$cluster_name/.
	sudo bash $cluster_base_directory/$cluster_name/edit_network_interfaces.sh
fi

