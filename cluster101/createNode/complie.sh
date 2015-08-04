gcc -std=c99 -o createNode createNode.c clusterCfgRead.c newCluster_KVM.c db.c createDisk.c `mysql_config --cflags --libs`
