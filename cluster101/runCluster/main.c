#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pthread.h>

int clusterCfgRead();

extern char clusterHomeDir[256]; 
int nodes;
char **xml;

void *vm_thread(void *arg);
void user_thread();

static void customConnErrorFunc(void *userdata, virErrorPtr err)
{
	fprintf(stderr, "Failure of libvirt library call:\n");
	fprintf(stderr, " Code: %d\n", err->code);
	fprintf(stderr, " Domain: %d\n", err->domain);
	fprintf(stderr, " Message: %s\n", err->message);
	fprintf(stderr, " Level: %d\n", err->level);
	fprintf(stderr, " str1: %s\n", err->str1);
	fprintf(stderr, " str2: %s\n", err->str2);
	fprintf(stderr, " str3: %s\n", err->str3);
	fprintf(stderr, " int1: %d\n", err->int1);
	fprintf(stderr, " int2: %d\n", err->int2);
}
static void customGlobalErrorFunc(void *userdata, virErrorPtr err)
{
	fprintf(stderr, "Global handler, failure of libvirt library call:\n");
	fprintf(stderr, " Code: %d\n", err->code);
	fprintf(stderr, " Domain: %d\n", err->domain);
	fprintf(stderr, " Message: %s\n", err->message);
	fprintf(stderr, " Level: %d\n", err->level);
	fprintf(stderr, " str1: %s\n", err->str1);
	fprintf(stderr, " str2: %s\n", err->str2);
	fprintf(stderr, " str3: %s\n", err->str3);
	fprintf(stderr, " int1: %d\n", err->int1);
	fprintf(stderr, " int2: %d\n", err->int2);
}

int main()
{
	if(getuid()!=0)
	{
		fprintf(stdout,"\ntry with sudo");
		exit(0);
	}

	/*--------------- it is given here to make it thread safe ----------------------*/
	virConnectPtr conn;
	
	/* set a global error function for all connections */
	//virSetErrorFunc(NULL, customGlobalErrorFunc);

	
	conn = virConnectOpen("qemu:///system"); // xen:///
	if (conn == NULL) {
		fprintf(stderr, "Failed to open connection to qemu:///system\n");
		return 1;
	}
	
	/* conn will use a different error function  it has highest precedence*/
	//virConnSetErrorFunc(conn, NULL, customConnErrorFunc);
	
	/* capablity we only need cpus and memory	*/
	virNodeInfo nodeinfo;
	virNodeGetInfo(conn, &nodeinfo);
	
	printf("\033[2J"); //clear the screen
	printf("\033[H");

	virConnectClose(conn);
	/*------------------ it is given here to make it thread safe ----------------------*/

	/* read the config file */
	
	clusterCfgRead(); //dir
	char vms[16][256];
	
	char ffg[1024];
	sprintf(ffg,"%s/cluster.config",clusterHomeDir);
	
	FILE *f1;
	f1=fopen(ffg,"rw");
	
	if(f1==NULL)
	{
		perror("\nerror reading config file");
		exit(1);
	}
	char *line;
	line=(char *)malloc(128);
	size_t l=128;
	int i=0,j=0;
	printf("\nthese are the clusters already made");
	fflush(stdin);
	while (!feof(f1)) {
		if(getline(&line,&l,f1)!=-1)
		{
				strcpy(vms[i],line);
				j=strlen(vms[i]);
				if(vms[i][j-1]=='\n') //ending the last new line
					vms[i][j-1]=0;
		
				printf("\n%s",vms[i]);
				i++;	
		}
    }
    int n=i;
    
	fclose(f1);
	
	/* ask which cluster it wants to run  if does not exist tell him to run the other script*/
	char clus[256];
	memset(clus,0,256);
s1:	
	fprintf(stdout,"\nEnter one from above clusters  to run    ");
	fflush(stdin);
	scanf("%s",clus);
	/* error check */
	i=0;
	for(;i<n;i++)
	{
		if(strcmp(vms[i],clus)==0)
			break;
	}
	if(i==n) //nothing found
	{
		fprintf(stdout,"\nYou entered a wrong cluster [it does not exist]");
		goto s1;
	}
	/* read the no of vm it has */
	char dir[1024];
	sprintf(dir,"%s/%s/node.config",clusterHomeDir,clus);
	
	FILE *fp;
	fp=fopen(dir,"r");
	if(fp==NULL)
	{
		perror("\nerror reading config file");
		exit(0);
	}
	
	fscanf(fp,"%d",&nodes);
	fclose(fp);
	
	//printf("\n ----%d -------",nodes);		
	
	/*------------ now copy the xml from its files -----------*/
	xml=(char**)malloc(nodes*sizeof(char*));
	for(i=0;i<nodes;i++)
	{
		xml[i]=(char*)malloc(1024);
	}
	
	for(i=0;i<nodes;i++)
	{	
		FILE *fp;
		
		char x[1024];
		sprintf(x,"%s/%s/%d.xml",clusterHomeDir,clus,i);
		fp=fopen(x,"r");
		if(fp==NULL)
		{
			perror("\nInconsitencies found in config files");
			exit(0);
		}
		s2:
		if(fread(xml[i],1,1024,fp)==0)
		{
			if(errno=EINTR)
				goto s2;
			else
			{
				perror("\nerror reading xml file");
				exit(0);
			}
		}
		//fprintf(stdout,"\n%s\n",xml[i]);
	}
/* ---------------------  vm creation -----------------------------*/
	pthread_t *t;
	pthread_attr_t *a;
	
	t=(pthread_t*)malloc(nodes*sizeof(pthread_t));
	a=(pthread_attr_t*)malloc(nodes*sizeof(pthread_attr_t));
	
	int *arr_nodes;
	arr_nodes=(int*)malloc(sizeof(int)*nodes); //used to pass node no to threads
	
	for(i=0;i<nodes;i++)
	{
		int s;
		arr_nodes[i]=i;
		
		s=pthread_attr_init(&a[i]);
		if(s!=0)
		{
			perror("err in attr_init");
			exit(0);
		}
		
		pthread_create(&t[i],&a[i],&vm_thread,(void*)&arr_nodes[i]);
		
		//if(pthread_join(t[i],NULL)!=0)
		//perror("\nSomething went wrong");
	}
//-------------------------user interaction Thread--------------------------------------
	pthread_t ut;
	pthread_attr_t ua;
	
	
	
	int s;
	
	s=pthread_attr_init(&ua);
	if(s!=0)
	{
		perror("\nUnable to process any further [pyhread_attr_init]");
		//we should deallocate all the vms
		exit(0); 
	}
	
	//pthread_create(&ut,&ua,&user_thread,NULL);
	user_thread();
	//if(pthread_join(ut,NULL)!=0)
		//perror("\nSomething went wrong");
}
