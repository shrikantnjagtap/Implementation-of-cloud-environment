#include<stdio.h>
#include<unistd.h>
#include<pwd.h>
#include <stdlib.h>

int createDisk(char *dir,int i,int size);
int clusterCfgRead();
//const char *readLine(FILE *file);
int newCluster(char *dir,char *);

extern char clusterHomeDir[256];
	
int main()
{
	
	if(getuid()!=0)
	{
		printf("\ntry with sudo");
		exit(0);
	}
	int z=0;
	fprintf(stdout,"\nenter the 1 if you want continue creating a new cluster");
	fflush(stdin);
	scanf("%d",&z);
	if(z!=1)
		return 0;	
	/*  read the cluster dir in home file */
	clusterCfgRead();
	//printf("%s",clusterHomeDir);
	
	/* read the config file*/
	char cfg_file[512];
	
	memset(cfg_file,0,512);
	strcat(cfg_file,clusterHomeDir);
	strcat(cfg_file,"/cluster.config");
	printf("\n%s",cfg_file);
	
	FILE *f1;
	f1=fopen(cfg_file,"rw");
	
	if(f1==NULL)
	{
		perror("\nfolder\\file not found");
		exit(1);
	}
	char *line;
	line=(char*)malloc(128);
	
	size_t l=128;
	
	while (!feof(f1)) {
		if(getline(&line,&l,f1)!=-1)
		{
			printf("\nthese are the clusters already made %s", line);
		}
    }
     
	fclose(f1);
	/* ask for cluster name */
	fprintf(stdout,"\nenter the name of your cluster    ");
	char clus[256];
	fflush(stdin);
	scanf("%s",clus);

	/*  append the cluster.config file */
	char vms[16][256];
	
	char ffg[1024];
	sprintf(ffg,"%s/cluster.config",clusterHomeDir);
	
	//FILE *f1;
	f1=fopen(ffg,"r");
	
	if(f1==NULL)
	{
		perror("\nerror reading config file");
		exit(1);
	}
	//char *line;
	line=(char *)malloc(128);
	l=128;
	int i=0,j=0;
	fflush(stdin);
	while (!feof(f1)) {
		if(getline(&line,&l,f1)!=-1)
		{
				strcpy(vms[i],line);
				j=strlen(vms[i]);
				if(vms[i][j-1]=='\n') //ending the last new line
					vms[i][j-1]=0;
		
				//printf("\n%sd",vms[i]);
				i++;	
		}
    }
    int n=i;
    fclose(f1);
             /* now write back */
	f1=fopen(ffg,"w+");
	
	if(f1==NULL)
	{
		perror("\nerror reading config file");
		exit(1);
	}
    for(i=0;i<n;i++)
	{
		fprintf(f1,"%s\n",vms[i]);
	}
	fprintf(f1,"%s\n",clus);
	fclose(f1);
//------------------------------------------------------------------------

	char full_path[1024];
	memset(full_path,0,1024);
	strcpy(full_path,clusterHomeDir);
	strcat(full_path,"/");
	strcat(full_path,clus);
	
	
	newCluster(full_path,clus);
	
	

/*  ---------- now copy the -----ip confiurations to the vm disk -----------*/
	FILE *fpp;
	char command[1024];
	memset(command,0,1024);
	/* cd to the proper directory */
	sprintf(ffg,"%s/%s",clusterHomeDir,clus); //cluster directory
	sprintf(command,"cp edit_network_interfaces.sh %s",ffg); //copied to cluster dir
	system(command);
	
	sprintf(command,"cd %s && bash edit_network_interfaces.sh",ffg); //cd to that dir	
	system(command);
	
	
/*s1:	
	fpp=popen(command,"r");
	char cmd1[1024];
	
	if(fpp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd1);
		return;
		goto s1;
	}
	
	while(!feof(fpp))
	{
		if((fgets(cmd1,1024,fpp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd1);
			
		}
	}	
	pclose(fpp);*/
	
	
	return 0;
}

