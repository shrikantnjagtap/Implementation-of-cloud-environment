gcc -std=c99 -o runNode runNode.c clusterCfgRead.c db_hostName.c  `mysql_config --cflags --libs` -lvirt
