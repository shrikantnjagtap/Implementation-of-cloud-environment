#include<stdio.h>
#include<string.h>
int no_nodes;
char buffer[16][100];

/*char *getRunningNodes()
{
   char *p=(char*)malloc(256);
    strcpy(p,"node1 node2 node5 node4 node6");
    return p;
}*/
void read_node()
{
	int node_count=16,i=0,d=0;
		
	
	char *p;

	p=getRunningNodes();

	int t=0,s=0;	
	for(i=0;i<node_count;i++)
	{
		d=sscanf(p+t,"%s",buffer[i]);
		if(d!=1)
		{
			break;
		}
		//printf("\n%s",buffer[i]);
		s=strlen(buffer[i]);
		t=t+s+1;
	}
	//i--;
	no_nodes=i;
	printf("\n%d %d",i,no_nodes);	
}
