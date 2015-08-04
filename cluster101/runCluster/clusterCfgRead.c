#include<stdio.h>
#include<unistd.h>
#include<pwd.h>

char clusterHomeDir[256]; 

int clusterCfgRead()
{
	/* get the cluster home address */
	struct passwd *pw=getpwuid(getuid());
	const char *h=pw->pw_dir; //homedir
	
	memset(clusterHomeDir,0,256);
	
	strcpy(clusterHomeDir,"/home");
	strcat(clusterHomeDir,"/cluster101");
	
	//printf("\n%s",clusterHomeDir);
	
}

