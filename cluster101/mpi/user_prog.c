#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<string.h> 
void main(int argc,char **argv)
{
		
	int opt,no_nodes;
	char exe_cmd[100],tmp[10],mpi_exe_cmd[1024],exe_file[1024];	
	FILE *fin,*fout,*fp;	
	tmp[0]='\0';
	strcpy(exe_cmd,"EXEC_PROG ");


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
			system("mpirun -np 6 mpi_exe_1");	
			
			fin=fopen("mpi_executable_command.txt","r");
			if(fin==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
			fgets(mpi_exe_cmd,1024,fin);
			fclose(fin); 
			//printf("%s %s\n",mpi_exe_cmd,argv[1]);
			printf("%s\n",mpi_exe_cmd);
			/*fp=popen(mpi_exe_cmd,"r");	
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
			fclose(fout);*/	 
			break;
		case 2: strcat(exe_cmd,argv[1]);
			fin=fopen("simple_exe_file.txt","w");
			if(fin==NULL)
			{
				printf("\n invoking  failed:\n");
				return;
			}
			fprintf(fin,"%s",argv[1]);
			fclose(fin);	
			system("mpirun -np 6 user_exe4_1");
				
			break;
	}
		
	
}
