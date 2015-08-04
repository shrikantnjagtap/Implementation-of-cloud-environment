#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pthread.h>
#include <string.h>

int clusterCfgRead();
int query();

int user_id;
int clust_id;
int node_id;

extern char node_name[128];

extern char clusterHomeDir[256]; 

char xml[1024];


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

int main(int argc, char **argv)
{
/*	if(getuid()!=0)
	{
		fprintf(stdout,"\ntry with sudo");
		exit(0);
	} */

	if(argc < 4)
	{
		fprintf(stdout,"\nnot enough input");
		return 1;
	}
		
	user_id=atoi(argv[1]);
	clust_id=atoi(argv[2]);
	node_id=atoi(argv[3]);
	

	clusterCfgRead();
	
	/*	query the database for hostname */
	int r=0;
	r=query();
	
	if(r!=0)
	{
		//error
		exit(2);
	}
	
	/*--------------- it is given here to make it thread safe ----------------------*/
	virConnectPtr conn;
	
	/* set a global error function for all connections */
	//virSetErrorFunc(NULL, customGlobalErrorFunc);

	
	conn = virConnectOpen("qemu:///system"); // xen:///
	if (conn == NULL) {
		fprintf(stderr, "Failed to open connection to qemu:///system\n");
		return 9;
	}
	
	virConnSetErrorFunc(conn, NULL, customConnErrorFunc);
	
	/* check if it is already running */
	virDomainPtr dom;
	
	dom = virDomainLookupByName(conn, node_name);
	
	if(dom !=NULL)
		return 0;  //already running
		
	virConnectClose(conn);
	/*------------------ it is given here to make it thread safe ----------------------*/

	/*------------ now copy the xml from its file -----------*/
	
		FILE *fp;
		
		char x[1024];
		sprintf(x,"%s/%d/%d/%d.xml",clusterHomeDir,user_id,clust_id,node_id);
		fp=fopen(x,"r");
		if(fp==NULL)
		{
			perror("\nInconsitencies found in config files");
			exit(10);
		}
		s2:
		if(fread(xml,1,1024,fp)==0)
		{
			if(errno=EINTR)
				goto s2;
			else
			{
				perror("\nerror reading xml file");
				exit(11);
			}
		}
		fclose(fp);
	
/* ---------------------  vm creation -----------------------------*/
		
	
	
	if (virInitialize() < 0) {
         fprintf(stderr, "Failed to initialize libvirt");
         return 12;
     }


    conn = virConnectOpen("qemu:///system"); //xen+unix:///
    if (!conn) {
        printf("error opening\n");
        return 13;
    }

	dom=virDomainCreateXML(conn, xml,0 );
	if (!dom) {
		fprintf(stderr, "Domain creation failed");
		return 14; //return proper error
	}
	
	if (conn && virConnectClose(conn) < 0){
        printf("error closing\n");
    }

	
}
