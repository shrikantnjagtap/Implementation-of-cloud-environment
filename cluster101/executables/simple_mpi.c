#include <mpi.h>
#include <stdio.h>

char* getIP(const char* interface);


int main(int argc, char **argv)

{

	int *buf, i, rank, nints, len;

	char *IP;
	
	
	MPI_Init(&argc,&argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//IP=getIP("eth0");
	printf("Hello world!  I am process number: %d on host %s\n", rank, IP);

	MPI_Finalize();

	return 0;

}


