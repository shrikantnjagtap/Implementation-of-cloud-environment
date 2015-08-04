#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

int createDisk(char *dir,int i,int size);
int clusterCfgRead();
//const char *readLine(FILE *file);
int newNode(char *dir,int node_no);


int user_id;
int clust_id;
int node_id;


extern char clusterHomeDir[256];
	
int main(int argc, char **argv)
{
	
	/*  read the cluster dir in home file */
	clusterCfgRead();
	//printf("%s",clusterHomeDir);
	/* read the customer_id  cluster_no node_no*/
	if(argc < 4)
	{
		fprintf(stdout,"\nnot enough input");
		return 1;
	}
		
	user_id=atoi(argv[1]);
	clust_id=atoi(argv[2]);
	node_id=atoi(argv[3]);
	
	char user_id_str[256];
	memset(user_id_str,0,256);
	strcpy(user_id_str,argv[1]);
	
	char clust_id_str[256];
	memset(clust_id_str,0,256);
	strcpy(clust_id_str,argv[2]);
	
	//printf("\n%d %d %d",user_id,clust_id,node_id);
	
	
//------------------------------------------------------------------------

	char full_path[1024];
	memset(full_path,0,1024);

	strcpy(full_path,clusterHomeDir);
	strcat(full_path,"/");
	strcat(full_path,user_id_str);  //making the path of the user folder
	strcat(full_path,"/");
	strcat(full_path,clust_id_str);  //making the path of the user folder	
	
	newNode(full_path,node_id);
	
	

/*  ---------- now copy the -----ip confiurations to the vm disk -----------*/
	
	char command[1024];
	memset(command,0,1024);
	
	/* cd to the proper directory */
	
	if(node_id==1)
	{
		sprintf(command,"sudo cp %s/executables/masterNode.sh %s/",clusterHomeDir,full_path); //copied to cluster dir
		if(system(command)==-1)
			return 5;
	
		sprintf(command,"cd %s/ && bash masterNode.sh %d %d %d",full_path,user_id,clust_id,node_id); //cd to that dir	
		if(system(command)==-1)
			return 6;
	}
	else
	{
		sprintf(command,"sudo cp  %s/executables/clientNode.sh %s/",clusterHomeDir,full_path); //copied to cluster dir
		if(system(command)==-1)
			return 7;
	
		sprintf(command,"cd %s/ && bash clientNode.sh %d %d %d",full_path,user_id,clust_id,node_id); //cd to that dir	
		if(system(command)==-1)
			return 8;
	}
	

	
	return 0;
}

