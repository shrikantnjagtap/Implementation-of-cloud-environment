gcc -std=c99 -o editNode editNode.c clusterCfgRead.c editNode_KVM.c db.c editDisk.c `mysql_config --cflags --libs`
