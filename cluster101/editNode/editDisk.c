#include <stdio.h>
#include <string.h>

extern int user_id;
extern int clust_id;
extern int node_id;
extern char clusterHomeDir[256];
extern int disk_old_size;

int createDisk(char *dir,int i,int size)
{
	FILE *fp;
	char command[1024];
	memset(command,0,1024);

	char baseImg[128];
	strcpy(baseImg,"old.img");
	

	sprintf(command,"mv %s/%d.img  %s/old.img",dir,node_id,dir); 	
	fp=popen(command,"r");
	char cmd[1024];
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		exit(18);
	}
	
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd);
			
		}
	}
		
	pclose(fp);

s1:	
	sprintf(command,"virt-filesystems --long -h --all -a %s/%s",dir,baseImg); //clusterHomeDir=base dir
	fp=popen(command,"r");
	
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
	sprintf(command,"truncate -r %s/%s %s/%d.img",dir,baseImg,dir,i);
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
	sprintf(command,"truncate -s +%dG %s/%d.img",(size-disk_old_size),dir,i);
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
	sprintf(command,"virt-resize --expand /dev/sda1 %s/%s %s/%d.img",dir,baseImg,dir,i);
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
	
	sprintf(command,"rm -f  %s/old.img",dir);
	system(command);
	return 0;
}
