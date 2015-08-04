#include <my_global.h>
#include <mysql.h>
#include <string.h>

extern int user_id;
extern int clust_id;
extern int node_id;


int memory;
int vcpu;
int disk;
int up_bandwidth;
int down_bandwidth;
char node_name[1024];

char dbError[1024];

void finish_with_error(MYSQL *con)
{
	//sprintf(dbError, "%s\n", mysql_error(con));
	mysql_close(con);
	return 1; //error        
}

int query()
{
	MYSQL *con = mysql_init(NULL);
  
	if (con == NULL)
	{
		fprintf(stderr, "mysql_init() failed\n");
		return 1;
	}
	
	if (mysql_real_connect(con, "localhost", "root", "redhat", 
          "test", 0, NULL, 0) == NULL) 
	{
		finish_with_error(con);
	}      
	
	char qr[1024];
	memset(qr,0,1024);
	sprintf(qr,"SELECT vcpu, memory, disk, up_bw, down_bw, node_name FROM nodes where user_id=%d AND node_id=%d AND cluster_id=%d",user_id,node_id,clust_id);
	if (mysql_query(con,qr)) 
	{
		finish_with_error(con);
	}
	
	MYSQL_RES *result = mysql_store_result(con);
  
	if (result == NULL) 
	{
		finish_with_error(con);
	}

	int num_fields = mysql_num_fields(result);

	 
	if(num_fields != 6)
	{
		//error
		return 1;
	}
	
	MYSQL_ROW row;
	
	row = mysql_fetch_row(result);
	

	/*  store the values in the global variables    */
	vcpu=atoi(row[0]);
	memory=atoi(row[1]);
	disk=atoi(row[2]);
	up_bandwidth=atoi(row[3]);
	down_bandwidth=atoi(row[4]); 
	strcpy(node_name,row[5]);
		  
	mysql_free_result(result);
	mysql_close(con);
	
	return 0;
}
