#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <sys/signal.h>
#include <unistd.h>
#include <syslog.h>

int user_id;
int clust_id;
int node_id;

extern char node_name[128];

extern char clusterHomeDir[256]; 

int clusterCfgRead();

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
	if(argc < 4)
	{
		exit(1); 	//error
	}
	
	user_id=atoi(argv[1]);
	clust_id=atoi(argv[2]);
	node_id=atoi(argv[3]);
	
	clusterCfgRead();
	
	int r=0;
	r=query();
	
	if(r!=0)
	{
		//error
		exit(2);
	}
	
	virDomainPtr dom;
	
	//virSetErrorFunc(NULL, customGlobalErrorFunc);
	
	virConnectPtr conn;
	
	if (virInitialize() < 0) {
         fprintf(stderr, "Failed to initialize libvirt");
         return 12;
     }

	//virEventRegisterDefaultImpl();
	
    conn = virConnectOpen("qemu:///system"); //xen+unix:///
    if (!conn) {
        printf("error opening\n");
        return 13;
    }

	dom = virDomainLookupByName(conn, node_name);

	if(dom==NULL)
	{
		//error the vm is not running
		return 15; 	//return appropriate error
	}
		
	/*---------------	save it in it folder ----------------------------- */
	
	int s=0;
	int res=0;

		h_a1:
			if(virDomainGetState(dom,&s,&res,0)==-1)     //to check if it is already paused
			{
				goto h_a1;
			}
			else
			{
				//if paused show that it is paused, ask it to resume and than save 
				if(s == VIR_DOMAIN_PAUSED)
					exit(19);
			}	
	
	/* coming here we have to take the snap shot and restore it later  */
	
	// Build the path to save  saving as snap$.img [$=node_id]
	char path[512];
	memset(path,0,512);
	sprintf(path,"%s/%d/%d/snap%d.img",clusterHomeDir,user_id,clust_id,node_id);
	
	if( virDomainSave(dom,path)!=0)
	{
		exit(20);
	}
	
	
	
	/*----------------------------------------------------------	*/
	if (conn && virConnectClose(conn) < 0){
        printf("error closing\n");
        return 16;
    }
    return 0;
}

