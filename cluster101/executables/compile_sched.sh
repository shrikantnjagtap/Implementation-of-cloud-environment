mpicc -o mpi_parallel_schd mpi_schd.c
mpicc -o mpi_single_schd user_single.c
gcc -o sched sched.c GetRunningNodes.c read_node.c -lm
