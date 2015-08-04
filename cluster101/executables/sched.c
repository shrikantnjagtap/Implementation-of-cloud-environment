#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h> 


extern int no_nodes;
extern char buffer[16][100];




void main(int argc,char **argv)
{
		
	int opt,len,i;
	char exe_cmd[100]="mpirun -np ",tmp[10],mpi_exe_cmd[1024],exe_file[1024];	
	FILE *fin,*fout,*fp,*fnode;
	char output_file[100]="output_",cmd[1024];	
	

	printf("\n--------------------OPTION-------------------\n");
	printf("\n\t1.PARALLEL PROCESS NOTE:(It must be parallel executable program)\n\t2.SIMPLE PROCESS\n\t3.EXIT");
	while(1)
	{
		printf("\n\tENTER OPTION:");		
		scanf("%d",&opt);
		if(opt >= 1 && opt <= 2)
			break;
		else
			printf("\n ENTER VALID OPTION\n");
		
	}
	
/* rading the nodes which we r going to schedule the program */
	read_node();	
	
	
	fnode=fopen("nodes.txt","w");
	if(fnode==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	for(i=0;i<no_nodes;i++)
	{
		fprintf(fnode,"%s ",buffer[i]);
	}
	fclose(fnode);	


	printf("\n%d",no_nodes);
	snprintf(tmp, 10,"%d",no_nodes);
	strcat(exe_cmd,tmp);
	strcat(exe_cmd," -H ");
	for(i=0;i<no_nodes;i++)
	{
		strcat(exe_cmd,buffer[i]);
		strcat(exe_cmd,",");
	}
	len=strlen(exe_cmd)-1;
	exe_cmd[len]='\0';
	strcat(exe_cmd," ");
	printf("%s\n",exe_cmd);
	switch(opt)
	{
		case 1: printf("\n ENTER NUMBER OF INSTANCES YOU WANT TO EXECUTE: ");
			scanf("%d",&no_nodes);
			fin=fopen("papalel_exe_file.txt","w");
			if(fin==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
			fprintf(fin,"%s %d",argv[1],no_nodes);
			fclose(fin);
			
			strcat(exe_cmd,"mpi_parallel_schd");
			printf("\n%s",exe_cmd);
			system(exe_cmd);//call mpi scheduling 	
			
			fin=fopen("mpi_executable_command.txt","r");
			if(fin==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
			fgets(mpi_exe_cmd,1024,fin);
			fclose(fin); 
			printf("%s\n",mpi_exe_cmd);
			fp=popen(mpi_exe_cmd,"r");	
			if(fp==NULL)
			{
				printf("\n invoking failed:\n");
				return;
			}
			strcat(output_file,argv[1]);
			strcat(output_file,".txt");
			fout=fopen(output_file,"w");		
			if(fout==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
		
			while(!feof(fp))
			{
				if((fgets(cmd,1024,fp))!=NULL)
				{
					fprintf(fout,"%s",cmd);
			
				}
			}	
			fclose(fp);		
			fclose(fout); 
			break;
		case 2: 
			//strcat(exe_cmd,argv[1]);
			fin=fopen("simple_exe_file.txt","w");
			if(fin==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
			fprintf(fin,"%s",argv[1]);
			fclose(fin);	
			strcat(exe_cmd,"mpi_single_schd");
			system(exe_cmd);
			printf("%s",exe_cmd);	
			break;
	}
		
	
}
