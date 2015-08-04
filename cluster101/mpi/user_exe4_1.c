#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include<string.h> 

int main(int argc,char **argv)
{
    	int node_id, num_of_nodes;
    	int cpu_info,min_cpu_id=0,no_of_nodes;
	int exe_buffer_select[100];
	int buffer_cpu1[100],buffer_cpu2[100],buffer_cpu3[100],buffer_cpu_avg[100];
	int buffer_cmem1[100],buffer_cmem2[100],buffer_cmem3[100],buffer_cmem_avg[100];
	int buffer_tmem[100];
	int buffer_mem1[100],buffer_mem2[100],buffer_mem3[100],buffer_mem_avg[100];
	int buffer_qproc1[100],buffer_qproc2[100],buffer_qproc3[100],buffer_qproc_avg[100];
	int buffer_io1[100],buffer_io2[100],buffer_io3[100],buffer_io_avg[100];
	int i,min_cpu_node;
    	char executable_file[100],process_name[100],cmd[1024];
	FILE *fp,*fout,*fin;
	char tmp_fname[10]="./";	
	char output_file[100]="output_";
	int buffer_select[100],min_cpu;
	long double a[4],b[4],loadavg;
    	char dump[50];	


	fin=fopen("simple_exe_file.txt","r");
	if(fin==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	
	fscanf(fin,"%s",executable_file);	
	fclose(fin);	 
	printf("%s",executable_file);

	MPI_Init(&argc,&argv);
	
    	MPI_Comm_size(MPI_COMM_WORLD,&num_of_nodes);// to get taotal no of nodes
    	MPI_Comm_rank(MPI_COMM_WORLD,&node_id);
	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    	fclose(fp);
    	sleep(1);
    	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    	fclose(fp);
    	loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));	
	cpu_info=loadavg*100;
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cpu1, 1,MPI_INT, 0,MPI_COMM_WORLD); 
	//MPI_Barrier(MPI_COMM_WORLD);	
	
	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    	fclose(fp);
    	sleep(1);
    	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    	fclose(fp);
    	loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
	cpu_info=loadavg*100;
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cpu2, 1,MPI_INT, 0,MPI_COMM_WORLD); 
		
	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    	fclose(fp);
    	sleep(1);
    	fp = fopen("/proc/stat","r");
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    	fclose(fp);
    	loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
	cpu_info=loadavg*100;
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cpu3, 1,MPI_INT, 0,MPI_COMM_WORLD); 
			
			
	
	fp=popen("free -m| tail -3| awk '{print $3}' | head -1","r");// get current mem utilization	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cmem1, 1,MPI_INT, 0,MPI_COMM_WORLD); 
	
	fp=popen("free -m| tail -3| awk '{print $3}' | head -1","r");// get current mem utilization	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cmem2, 1,MPI_INT, 0,MPI_COMM_WORLD); 
	
	fp=popen("free -m| tail -3| awk '{print $3}' | head -1","r");// get current mem utilization	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_cmem3, 1,MPI_INT, 0,MPI_COMM_WORLD); 
		
	
	fp=popen("free -m| tail -3| awk '{print $2}' | head -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_tmem, 1,MPI_INT, 0,MPI_COMM_WORLD); 
	
	fp=popen("vmstat |awk '{print $1}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_qproc1, 1,MPI_INT, 0,MPI_COMM_WORLD); 
		
	fp=popen("vmstat |awk '{print $1}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_qproc2, 1,MPI_INT, 0,MPI_COMM_WORLD); 
		
	fp=popen("vmstat |awk '{print $1}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_qproc3, 1,MPI_INT, 0,MPI_COMM_WORLD); 
	
	


	fp=popen("vmstat |awk '{print $9}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_io1, 1,MPI_INT, 0,MPI_COMM_WORLD); 	

	fp=popen("vmstat |awk '{print $9}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_io2, 1,MPI_INT, 0,MPI_COMM_WORLD); 	
	
	fp=popen("vmstat |awk '{print $9}'|tail -1","r");// get total mem 	
	if(fp==NULL)
	{
		printf("\n invoking  failed:\n");
		return;
	}
	fscanf(fp,"%d",&cpu_info);
	//cpu_info=100-cpu_info;	
	fclose(fp);	
	MPI_Gather( &cpu_info,1, MPI_INT,buffer_io3, 1,MPI_INT, 0,MPI_COMM_WORLD); 	
	
	
		
	MPI_Barrier(MPI_COMM_WORLD);
	if(node_id==0)
	{
	
		for(i=0;i<num_of_nodes;i++)
		{
		        buffer_cmem_avg[i]=(buffer_cmem1[i]+buffer_cmem2[i]+buffer_cmem3[i])/3;	
		}
		

		for(i=0;i<num_of_nodes;i++)
		{
	        	buffer_cpu_avg[i]=(buffer_cpu1[i]+buffer_cpu2[i]+buffer_cpu3[i])/3;
		}		
		for(i=0;i<num_of_nodes;i++)
		{
		       	 buffer_qproc_avg[i]=(buffer_qproc1[i]+buffer_qproc2[i]+buffer_qproc3[i])/3;
		}
		for(i=0;i<num_of_nodes;i++)
		{
	        	buffer_io_avg[i]=(buffer_io1[i]+buffer_io2[i]+buffer_io3[i])/3;
		}		
		
		for(i=0;i<num_of_nodes;i++)
		{
			buffer_mem_avg[i]=(buffer_cmem_avg[i]*100/buffer_tmem[i]);				
		}	
		for(i=0;i<num_of_nodes;i++)
		{
			buffer_select[i]=(buffer_cpu_avg[i]+buffer_mem_avg[i]+buffer_qproc_avg[i]+buffer_io_avg[i])/4;	
		}	
		min_cpu=buffer_select[0];		
		for(i=0;i<num_of_nodes;i++)
		{
						
			if(min_cpu>buffer_select[i])
			{
				min_cpu=buffer_select[i];				
				min_cpu_id=i;
			}
		}	
		printf("Processor %d has data: ", node_id);
        	for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_cpu_avg[i]);
        	printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_cmem1[i]);
		printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_tmem[i]);
		printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_mem_avg[i]);
		printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_qproc1[i]);
		printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_io1[i]);
		printf("\n");
		for(i=0; i<num_of_nodes; i++)
            		printf("%d ", buffer_select[i]);
		printf("\n");		
		printf("%d\n",min_cpu_id);
	}
	if(node_id==min_cpu_id)
	{
		printf("\n program sent to :%d\n",min_cpu_id);		
		strcat(tmp_fname,executable_file); 
		fp=popen(tmp_fname,"r");	
		if(fp==NULL)
		{
			printf("\n invoking failed:\n");
			return;
		}
		strcat(output_file,executable_file);
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
	}			
	MPI_Finalize();		
}

