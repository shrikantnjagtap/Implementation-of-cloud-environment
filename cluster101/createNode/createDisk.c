#include <stdio.h>
#include <string.h>

extern int user_id;
extern int clust_id;
extern int node_id;
extern char clusterHomeDir[256];


int createDisk(char *dir,int i,int size)
{
	FILE *fp;
	char baseImg[128];
	
	if(node_id==1)
	{
		strcpy(baseImg,"baseVMMaster.img");
	}
	else
	{
		strcpy(baseImg,"baseVMClient.img");
	}
	char command[1024];
	memset(command,0,1024);
s1:	
	sprintf(command,"virt-filesystems --long -h --all -a %s/%s",clusterHomeDir,baseImg); //clusterHomeDir=base dir
	fp=popen(command,"r");
	char cmd[1024];
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		exit(4);
		goto s1;
	}
	
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd);
			
		}
	}
		
	pclose(fp);
	
s2:
	sprintf(command,"truncate -r %s/%s %s/%d.img",clusterHomeDir,baseImg,dir,i);
	fp=popen(command,"r");
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		exit(4);
		goto s2;
	}
	
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			fprintf(stdout,"%s",cmd);

		}
	}
		
	pclose(fp);
	
s3:
	sprintf(command,"truncate -s +%dG %s/%d.img",(size-2),dir,i);
	fp=popen(command,"r");
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		exit(4);
		goto s3;
	}
	
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd);
			
		}
	}	
	pclose(fp);	
	
s4:
	sprintf(command,"virt-resize --expand /dev/sda1 %s/%s %s/%d.img",clusterHomeDir,baseImg,dir,i);
	fp=popen(command,"r");
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		exit(4);
		goto s4;
	}
	
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd);
			
		}
	}
		
	pclose(fp);
	return 0;
}
