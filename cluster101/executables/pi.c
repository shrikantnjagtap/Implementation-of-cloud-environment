#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

char* getIP(const char* interface);

int main(int argc, char *argv[]) {

  int myid,nprocs;
 	char *IP;
  long int npts = 10000000000;

  long int i,mynpts;

  double f,sum,mysum;
  double xmin,xmax,x;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	IP=getIP("eth0");
  if (myid == 0) {
    mynpts = npts - (nprocs-1)*(npts/nprocs);
  } else {
    mynpts = npts/nprocs;
  }

  mysum = 0.0;
  xmin = 0.0;
  xmax = 1.0;

  srand(myid);

  for (i=0; i<mynpts; i++) {
    x = (double) rand()/RAND_MAX*(xmax-xmin) + xmin;
    mysum += 4.0/(1.0 + x*x);
  }

  printf("\nsending from ip=%s mysum=%l",IP,mysum);
  MPI_Reduce(&mysum,&sum,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  
  if (myid == 0) {
    f = sum/npts;
    printf("PI calculated with %ld points = %f \n",npts,f);
  }

  MPI_Finalize();


}
