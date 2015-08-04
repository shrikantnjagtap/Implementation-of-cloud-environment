#include <stdio.h>

extern char clusterHomeDir[256];

int createDisk(char *dir,int i,int size)
{
	FILE *fp;

	char command[1024];
	memset(command,0,1024);
s1:	
	sprintf(command,"virt-filesystems --long -h --all -a %s/baseVM.img",clusterHomeDir); //clusterHomeDir=base dir
	fp=popen(command,"r");
	char cmd[1024];
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		return;
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
	sprintf(command,"truncate -r %s/baseVM.img %s/%d.img",clusterHomeDir,dir,i);
	fp=popen(command,"r");
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
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
		return;
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
	sprintf(command,"virt-resize --expand /dev/sda1 %s/baseVM.img %s/%d.img",clusterHomeDir,dir,i);
	fp=popen(command,"r");
	
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		return;
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
}
