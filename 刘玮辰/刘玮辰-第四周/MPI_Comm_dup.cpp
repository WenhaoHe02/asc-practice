#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv)
{
	int myid, numprocs;
	MPI_Comm new_comm; 
	int result;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    // your code here
	MPI_Group first, second;
	int ranks[4]={0,2,0,0};
	MPI_Comm_group(MPI_COMM_WORLD, &first);
	MPI_Group_incl( first ,2 , ranks , &second);
	MPI_Comm_create( MPI_COMM_WORLD , second , &new_comm);
	// end of your code
		if(new_comm!=MPI_COMM_NULL)
	MPI_Comm_compare(MPI_COMM_WORLD, new_comm, &result);
	
	if(myid == 0&&new_comm!=MPI_COMM_NULL) {
		
		if ( result == MPI_IDENT) {
			printf("The comms are identical.\n");
		}
		else if ( result == MPI_CONGRUENT ) {
			printf("The comms are congruent.\n");
		}
		else if ( result == MPI_SIMILAR ) {
			printf("The comms are similar.\n");
		}
		else if ( result == MPI_UNEQUAL ) {
			printf("The comms are unequal.\n");
		}
	}

	MPI_Finalize();
	return 0;
}
