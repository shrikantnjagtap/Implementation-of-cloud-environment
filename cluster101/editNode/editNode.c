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
int disk_change;
int disk_old_size;

extern char clusterHomeDir[256];
	
int main(int argc, char **argv)
{
	
	/*  read the cluster dir in home file */
	clusterCfgRead();
	//printf("%s",clusterHomeDir);
	/* read the customer_id  cluster_no node_no*/
	if(argc < 5)
	{
		fprintf(stdout,"\nnot enough input");
		return 1;
	}
		
	user_id=atoi(argv[1]);
	clust_id=atoi(argv[2]);
	node_id=atoi(argv[3]);
	
	disk_change=atoi(argv[4]);
	disk_old_size=atoi(argv[5]);
	
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
	
	
	
	return 0;
}

