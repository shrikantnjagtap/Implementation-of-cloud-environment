#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <sys/signal.h>
#include <unistd.h>
#include <syslog.h>


#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED __attribute__((__unused__))
#endif


extern char **xml;
extern int nodes;

int validDom[16]; //to store which has a valid Domain ptr

virDomainPtr dPtr[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int unpause; //this will say how many to unpause
int pause_vms[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //if 0 bit is set 0th vm will be paused

//int run=1;

//int boot=0;

const char *eventToString(int event) {
    const char *ret = "";
    switch ((virDomainEventType) event) {
        case VIR_DOMAIN_EVENT_DEFINED:
            ret ="Defined";
            break;
        case VIR_DOMAIN_EVENT_UNDEFINED:
            ret ="Undefined";
            break;
        case VIR_DOMAIN_EVENT_STARTED:
            ret ="Started";
            break;
        case VIR_DOMAIN_EVENT_SUSPENDED:
            ret ="Suspended";
            break;
        case VIR_DOMAIN_EVENT_RESUMED:
            ret ="Resumed";
            break;
        case VIR_DOMAIN_EVENT_STOPPED:
            ret ="Stopped";
            break;
        case VIR_DOMAIN_EVENT_SHUTDOWN:
            ret = "Shutdown";
            break;
    }
    return ret;
}

static const char *eventDetailToString(int event, int detail) {
    const char *ret = "";
    switch ((virDomainEventType) event) {
        case VIR_DOMAIN_EVENT_DEFINED:
            if (detail == VIR_DOMAIN_EVENT_DEFINED_ADDED)
                ret = "Added";
            else if (detail == VIR_DOMAIN_EVENT_DEFINED_UPDATED)
                ret = "Updated";
            break;
        case VIR_DOMAIN_EVENT_UNDEFINED:
            if (detail == VIR_DOMAIN_EVENT_UNDEFINED_REMOVED)
                ret = "Removed";
            break;
        case VIR_DOMAIN_EVENT_STARTED:
            switch ((virDomainEventStartedDetailType) detail) {
            case VIR_DOMAIN_EVENT_STARTED_BOOTED:
                ret = "Booted";
                break;
            case VIR_DOMAIN_EVENT_STARTED_MIGRATED:
                ret = "Migrated";
                break;
            case VIR_DOMAIN_EVENT_STARTED_RESTORED:
                ret = "Restored";
                break;
            case VIR_DOMAIN_EVENT_STARTED_FROM_SNAPSHOT:
                ret = "Snapshot";
                break;
            }
            break;
        case VIR_DOMAIN_EVENT_SUSPENDED:
            switch ((virDomainEventSuspendedDetailType) detail) {
            case VIR_DOMAIN_EVENT_SUSPENDED_PAUSED:
                ret = "Paused";
                break;
            case VIR_DOMAIN_EVENT_SUSPENDED_MIGRATED:
                ret = "Migrated";
                break;
            case VIR_DOMAIN_EVENT_SUSPENDED_IOERROR:
                ret = "I/O Error";
                break;
            case VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG:
                ret = "Watchdog";
                break;
            case VIR_DOMAIN_EVENT_SUSPENDED_RESTORED:
                ret = "Restored";
                break;
            case VIR_DOMAIN_EVENT_SUSPENDED_FROM_SNAPSHOT:
                ret = "Snapshot";
                break;
            }
            break;
        case VIR_DOMAIN_EVENT_RESUMED:
            switch ((virDomainEventResumedDetailType) detail) {
            case VIR_DOMAIN_EVENT_RESUMED_UNPAUSED:
                ret = "Unpaused";
                break;
            case VIR_DOMAIN_EVENT_RESUMED_MIGRATED:
                ret = "Migrated";
                break;
            case VIR_DOMAIN_EVENT_RESUMED_FROM_SNAPSHOT:
                ret = "Snapshot";
                break;
            }
            break;
        case VIR_DOMAIN_EVENT_STOPPED:
            switch ((virDomainEventStoppedDetailType) detail) {
            case VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN:
                ret = "Shutdown";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_DESTROYED:
                ret = "Destroyed";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_CRASHED:
                ret = "Crashed";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_MIGRATED:
                ret = "Migrated";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_SAVED:
                ret = "Failed";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_FAILED:
                ret = "Failed";
                break;
            case VIR_DOMAIN_EVENT_STOPPED_FROM_SNAPSHOT:
                ret = "Snapshot";
                break;
            }
            break;
        case VIR_DOMAIN_EVENT_SHUTDOWN:
            switch ((virDomainEventShutdownDetailType) detail) {
            case VIR_DOMAIN_EVENT_SHUTDOWN_FINISHED:
                ret = "Finished";
                break;
            }
            break;
    }
    return ret;
}




static void sighndlr(int sig)
{
	/* send shutdown signal to all vm present */
	static no=0;
	if(no>=1) //already shuted down
		exit(0);
	
	int i;
	for(i=0;i<nodes;i++)
	{
		int s=0;
		int res=0;
	   h_a:
	   //printf("\n %d %d\n",i,nodes);
		if(validDom[i]==1)
		{
			if(virDomainGetState(dPtr[i],&s,&res,0)==-1) // if paused unpause it
			{
				//error
				goto h_a;
			}
			else
			{
	    h_s:
				if(s == VIR_DOMAIN_PAUSED)
				{
					if(virDomainResume(dPtr[i])==-1)
						goto h_s;
				}
					
			}
			virDomainShutdown(dPtr[i]);	
			validDom[i]=0;
		}			
	}
	no++;
	exit(0); //exit the program
}
static void sigUnpauseHdlr(int sig)
{
	int s=0;
	int res=0;
	printf("\nunpusing %d",unpause);
	int i=0;
	for(; i< unpause; i++) //assuimg all the vms r paused than unapusing those which r required
	{	
	   h_a:
		if(virDomainGetState(dPtr[i],&s,&res,0)==-1)
		{
			//error
			goto h_a;
		}
		else
		{
	    h_s:
			if(s == VIR_DOMAIN_PAUSED)
				if(virDomainResume(dPtr[i])==-1)
					goto h_s;
		}		
	}	
	
	return;
}
static void sigPauseHdlr(int sig)
{
	int s=0;
	int res=0;

	int i=0;
	for(;i<nodes;i++)
	{
		if(pause_vms[i] == 1) //if the element in the array is 1 pause
		{
			h_a1:
			if(virDomainGetState(dPtr[i],&s,&res,0)==-1)
			{
				goto h_a1;
			
			}
			else
			{
			h_s1:
				if(s != VIR_DOMAIN_PAUSED)
					if(virDomainSuspend(dPtr[i])==-1)
						goto h_s1;
			}	
		pause_vms[i]=0;
		}
	}
	
	return;
}

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

void *vm_thread(void *arg)
{
	int *i;
	i=(int *)arg;
	//int callbackret = -1;
	
	virDomainPtr dom;
	
	//printf("\n%s\n",xml[*i]);
	//fflush(stdout);
	
	virConnectPtr conn;
	
	if (virInitialize() < 0) {
         fprintf(stderr, "Failed to initialize libvirt");
         return -1;
     }

	//virEventRegisterDefaultImpl();
	
    conn = virConnectOpen("qemu:///system"); //xen+unix:///
    if (!conn) {
        printf("error opening\n");
        return -1;
    }

	dom=virDomainCreateXML(conn, xml[*i],0 );
	if (!dom) {
		fprintf(stderr, "Domain creation failed");
		return;
	}

	/*----------- storing the virDomainPtr for future use ----------*/
	dPtr[*i]=dom;  
	validDom[*i]=1;
	
	//fprintf(stderr, "Guest %s%d has booted","ubuntuxen",*i);
		
	//virDomainFree(dom); it is free ing the diomain ptr so the handler is not finding
	
	if (conn && virConnectClose(conn) < 0){
        printf("error closing\n");
    }
	pthread_exit(0);
}

char command[1024];

void create_command()
{
	
	char file_name[1024];
	char tmp_process[1024];
	int t=0,n=0;
	int nodes1,process,i,j,k,m,p;
	char c;
	char temp[256];
	/*while((c=getchar())!= '\n' && c!=EOF)
	{
	}*/

ll1:
	printf("\n[CTRL+C to EXIT] Enter  1  to  submit  single		jobs");
	printf("\n                 Enter  2  to  submit  parallel   jobs            "); 
	fflush(stdin);
	fflush(stdout);
	/*if(scanf("%d",&m)<=0)
	{
		printf("\nerror in input try again");
		goto ll1;
	}*/
	if(fgets(temp,256,stdin))
	{
		if((sscanf(temp,"%d",&m))!=1)
		{
			printf("\nError in input ");
			goto ll1;
		}
	}
	
	switch(m)
	{
		case 1:  //ssh to single nodes
			memset(command,0,1024);
			n=0,t=0;
			n=sprintf(command+t,"ssh ");
			t=t+n;
		ll2:
			//fflush(stdin);
			/*while((c=getchar())!= '\n' && c!=EOF)
			{
			}*/
			printf("\n Enter the node name 1 to %d to run   ",nodes);
			/*if(((scanf("%d",&p)<=0)) || p>=nodes+1 || p<=0 )
			{
				printf(" error  in reading inputs	");
				goto ll2;
			}*/
			if(fgets(temp,256,stdin))
			{
				if((sscanf(temp,"%d",&p))!=1)
				{
					printf("\nError in input ");
					goto ll2;
				}
				if(p>=nodes+1 || p<=0)
				{
					printf("\nError in input inputs out of bound");
					goto ll2;
				}
			}
			n=sprintf(command+t,"node%d ",p);
			t=t+n;
			printf("\n Enter the command   ");
			fflush(stdin);
			/*while((c=getchar())!= '\n' && c!=EOF)
			{
			}*/
			char *c;
			size_t t=256;
			c=(char *)malloc(256);
			fgets(c,256,stdin);
			m=strlen(c);
			if(c[m-1]=='\n')
				c[m-1]=0;
			//printf("\n%s ",c);
			//fflush(stdout);			
			//n=sprintf(command+t,"%s",c);
			//t=t+n;
			strcat(command,c);
			//	printf("\n%s ",command);
			/* unpause the vm  [pause only one vm]*/		
			unpause=p;
			raise(SIGUSR1);
			sleep(1);
			break;
		case 2:
			n=0;t=0;
			memset(command,0,1024);
			n=sprintf(command+t,"mpirun -np ");
			t=t+n;
			printf("\n [CTRL+C to exit]  Enter Executable file name: ");
			fflush(stdin);
			scanf("%s",file_name);
			printf("\n %s ",file_name);
			fflush(stdout);
			while(1)
			{
				fflush(stdin);
				printf("\n Enter number of nodes:");
				scanf("%d",&nodes1);
				//nodes=tmp_num-'0';			
				if((nodes1>=17)||(nodes1<=0))
				{
					printf("INVALID INPUT. PLEASE TRY AGAIN!\n");
					//nodes1=1;			
				}
				else
					break;
				fflush(stdout);
				fflush(stdin);
			}
		
			//fflush(stdin);
			//sprintf(tmp_nodes,"%d",nodes);
			while(1)
			{
				printf("\n Enter no of processes to be created:");	
				fflush(stdin);
				scanf("%d",&process);
				if(process<=0)
				printf(" INVALID INPUT. PLEASE TRY AGAIN!\n");
				else
					break;	
			}
	
			n=sprintf(command+t,"%d -H ",process);
			t=t+n;

			//printf("%s\n",comand);	
			j=1;
			while(j<=nodes1)
			{
				if(j==nodes1)
				{
					n=sprintf(command+t,"node%d ",j);
					t=t+n;
		
				}
				else
				{
					n=sprintf(command+t,"node%d,",j);
					t=t+n;
				}
				j++;
			}
	
			n=sprintf(command+t,"/home/deben/%s ",file_name);
			t=t+n;
		
			printf("%s\n",command);
			/* unpause the vms */		
			unpause=nodes1;
			raise(SIGUSR1);
			sleep(1);
			break; 	 /*--- end  of case 2 */
	}
	
			
}
void user_thread()
{
	FILE *fp,*fout;
	
	struct sigaction act;
	memset(&act, 0, sizeof act);	

	act.sa_handler=sighndlr;

	act.sa_flags= 0;
	
	if(sigaction(SIGINT,&act,NULL) < 0)	 // for ctrl+c
	{
		perror("sigaction");
		exit(0);
	}
		
	memset(&act, 0, sizeof act);	

	act.sa_handler=sigUnpauseHdlr;

	act.sa_flags= 0;
	
	if(sigaction(SIGUSR1,&act,NULL) < 0)	 // for user level handler
	{
		perror("sigaction");
		exit(0);
	}
	memset(&act, 0, sizeof act);	

	act.sa_handler=sigPauseHdlr;

	act.sa_flags= 0;
	
	if(sigaction(SIGUSR2,&act,NULL) < 0)	 // for user level handler
	{
		perror("sigaction");
		exit(0);
	}

	
	char cmd[1024];
     
	//stop execution untill all the vms are up and running//
	fprintf(stdout,"\nwaiting for  %d seconds to boot  the vms",nodes*45);
	fflush(stdout);
	sleep(45*nodes); //4 minutes for 4 vms
	
	int k=0,d=0;
	int state;
	int reason; 
		
	/* drop root user privileges*/
	
	/* build the pause_vms array */
	int j=0;
	/*for(;j<nodes;j++)
		pause_vms[j]=1; //all vms
	raise(SIGUSR2); //raise pause handler
	*/

	if(getuid()==0)
		setuid(1000);
	else
		setuid(1000);
	char c;
	while((c=getchar())!= '\n' && c!=EOF)
	{
	}
cmd_label:
	//pause all vms
	for(j=0;j<nodes;j++)
		pause_vms[j]=1; //all vms
	raise(SIGUSR2);
	
	create_command();

	printf("\n%s",command);
	fflush(stdout);
	fp=popen(command,"r");
	if(fp==NULL)
	{
		printf("\n invoking %s failed:\n",cmd);
		return;
	}
	fprintf(stdout,"\n");
	while(!feof(fp))
	{
		if((fgets(cmd,1024,fp))!=NULL)
		{
			
			fprintf(stdout,"%s",cmd);
			
		}
	}	
	pclose(fp);
	
	//memset(command,0,1024);
	goto cmd_label;
	return;

}
