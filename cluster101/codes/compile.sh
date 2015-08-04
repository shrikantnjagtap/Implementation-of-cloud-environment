gcc -o pauseNode pauseNode.c db_vmName.c -lvirt `mysql_config --cflags --libs`
gcc -o resumeNode resumeNode.c db_vmName.c -lvirt `mysql_config --cflags --libs`
gcc -o stopNode stopNode.c db_vmName.c -lvirt `mysql_config --cflags --libs`
gcc -o saveNode saveStateNode.c db_vmName.c clusterCfgRead.c -lvirt `mysql_config --cflags --libs`
gcc -o restoreNode restoreNode.c clusterCfgRead.c -lvirt 
