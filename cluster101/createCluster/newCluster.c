#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pthread.h>

int createDisk(char *dir/* new cluster dir*/,int i,int size);

extern char clusterHomeDir[256];

char **xml;

int nodes;


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

int newCluster(char *dir,char *cluster_name)
{
	char cmd[1024];
	
	sprintf(cmd,"mkdir  %s",dir);
	system(cmd);
	/* no need to copy the base image 
	 * 
	fprintf(stdout,"\ncopying the base image");
	sprintf(cmd,"cp %s/baseVM.img %s/baseVM.img",clusterHomeDir,dir);
	system(cmd);
	 */
	 
	//seteuid(0);
	/*             user input for no of nodes etc   */
	virConnectPtr conn;
	
	/* set a global error function for all connections */
	virSetErrorFunc(NULL, customGlobalErrorFunc);

	
	conn = virConnectOpen("qemu:///system"); // xen:///
	if (conn == NULL) {
		fprintf(stderr, "Failed to open connection to qemu:///system\n");
		return 1;
	}
	
	
	/* conn will use a different error function  it has highest precedence*/
	virConnSetErrorFunc(conn, NULL, customConnErrorFunc);
	
	/* capablity we only need cpus and memory	*/
	virNodeInfo nodeinfo;
	virNodeGetInfo(conn, &nodeinfo);
	
	printf("\033[2J"); //clear the screen
	printf("\033[H");
	
	/* 	get the user inputs   ----------------------------------*/
	int *ram;
	ram=(int*)malloc(sizeof(int)*nodes);
	int remaining_RAM=nodeinfo.memory/1024 - 1024; //for checking available memory
	int *disk;
	disk=(int*)malloc(sizeof(int)*nodes);
	int i=0;
	
	node_label:
	fprintf(stdout,"\n---------Enter no of nodes you want, no more than %d----------\n",16);
	if((scanf("%d",&nodes))<1)
	{
		printf("\nwrong input");
		goto node_label;
	}
	fflush(stdin);
	if( nodes > 16 || nodes<=0)
	{
		fprintf(stdout,"\nthe entered no of nodes is not within the range");
		goto node_label;
	}

	/* print to file the no of nodes */
	FILE *ffp;
	char ccc[234];
	

	sprintf(ccc,"%s/node.config",dir);
	ffp=fopen(ccc,"w+");
	if(ffp==NULL)
	{
		perror("unable to create the node.config file");
		exit(0);
	}
	fprintf(ffp,"%d",nodes);
	fclose(ffp);

		/*  ask for amount of ram for each nodes  */
	for(;i<nodes;i++)
	{
	ram_node:
		fprintf(stdout,"\n\t\t\tENTER AMOUNT OF RAM [in MB] FOR NODE %d NOT MORE THAN %d MB:",i,remaining_RAM);
		if((scanf("%d",&ram[i]))<1)
		{
			printf("\n\t\t\tWRONG INPUT");
			goto ram_node;
		}
		fflush(stdin);
		if(ram[i]>remaining_RAM || ram[i] <= 128)
		{
			fprintf(stdout,"\n\t\t\tYOU MAY HAVE ENTERED WRONG AMOUNT/RAM NOT AVAILABLE!");
			goto ram_node;
		}
		remaining_RAM=remaining_RAM-ram[i];
	  disk_node:	
		fprintf(stdout,"\n\t\t\tENTER AMOUNT OF DISK [in GB] FOR NODE %d:",i);
		if((scanf("%d",&disk[i]))<1)
		{
			printf("\n\t\t\tWRONG INPUT");
			goto disk_node;
		}
		fflush(stdin);
		if(disk[i] <= 1 || disk[i] >= 21)
		{
			fprintf(stdout,"\n\t\t\tPLEASE ENTER DISK SIZE LESS THAN 20 GB and GREATER THAN 2GB");
			goto disk_node;
		}
		
	}
	
	virConnectClose(conn);
	
	/*	-------------------  xml creation         ---------------------------------*/	
	xml=(char**)malloc(nodes*sizeof(char*));
	for(i=0;i<nodes;i++)
	{
		xml[i]=(char*)malloc(1024);
	}
	
	
		
	int total=0,n=0;
	
	
	for(i=0;i<nodes;i++)
	{
		total=0;n=0;
		
		n=sprintf(xml[i]+total,"<domain type='xen'>");
		total=total+n;
	
		n=sprintf(xml[i]+total,"<name>%s%s%d</name>","ubuXen",cluster_name,i);
		total=total+n;
	
		n=sprintf(xml[i]+total,"<os>");
		total=total+n;
	
			n=sprintf(xml[i]+total,"<type>linux</type>");
			total=total+n;
			n=sprintf(xml[i]+total,"<bootloader>/etc/alternatives/xen-default/bin/pygrub</bootloader>");
			total=total+n;

		n=sprintf(xml[i]+total,"</os>");
		total=total+n;

		n=sprintf(xml[i]+total,"<vcpu>1</vcpu>");
		total=total+n;
		n=sprintf(xml[i]+total,"<memory unit='KiB'>%d</memory>",ram[i]*1024);
		total=total+n;
	
		n=sprintf(xml[i]+total,"<devices>");
		total=total+n;
				n=sprintf(xml[i]+total,"<disk type='file'>");
				total=total+n;
					n=sprintf(xml[i]+total,"<source file='%s/%d%s'/>",dir,i,".img"); /* xxx/1.img*/
					
					createDisk(dir,i,disk[i]); //------------------------------------------//
					
					total=total+n;
					n=sprintf(xml[i]+total,"<target dev='sda'/>");
					total=total+n;
				n=sprintf(xml[i]+total,"</disk>");
				total=total+n;
			
				n=sprintf(xml[i]+total,"<interface type='bridge'>");
				total=total+n;
					n=sprintf(xml[i]+total,"<source bridge='xenbr0'/>"); /** change accordingly**/
					total=total+n;
					n=sprintf(xml[i]+total,"<mac address='DE:BE:21:22:11:0%d'/>",i);
					total=total+n;
				n=sprintf(xml[i]+total,"</interface>");
				total=total+n;
		n=sprintf(xml[i]+total,"</devices>");
		total=total+n;
		n=sprintf(xml[i]+total,"</domain>");
		total=total+n;
		//printf("\n%s\n",xml[i]);
		/*    save the xml  */
		FILE *fx;
		char fxml[512];
		memset(fxml,0,512);
		sprintf(fxml,"%s/%d.xml",dir,i);
		fx=fopen(fxml,"w");
		if(fx==NULL)
		{
			perror("\ncreating xml config");
			exit(0);
		}
		fprintf(fx,"%s",xml[i]);
		fclose(fx);
		
	}
	return 0;
}
