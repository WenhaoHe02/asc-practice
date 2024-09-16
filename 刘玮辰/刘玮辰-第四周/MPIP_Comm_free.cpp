#include<cstdio>
#include<mpi.h>

int main(int argc, char **argv)
{
	int myid, numprocs;
	MPI_Comm new_comm;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	MPI_Comm_dup(MPI_COMM_WORLD, &new_comm);
	
	// your code here
	MPI_Comm_free(&new_comm);
	// end of your code
	
	if(myid == 0) {
		if (new_comm == MPI_COMM_NULL)
			printf("Now the comm is freed.\n");
	}

	MPI_Finalize();
	return 0;
}

