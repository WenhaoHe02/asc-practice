#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int myid, numprocs, source;
	MPI_Status status;
	int i, j, position;
	int k[2];
	int buf[1000];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
    i = 1;
    j = 2;
    
    if(myid == 0) {
        
        position = 0 ;
        
    	MPI_Pack(&i, 1, MPI_INT, buf, 1000, &position, MPI_COMM_WORLD); 
        MPI_Pack(&j, 1, MPI_INT, buf, 1000, &position, MPI_COMM_WORLD); 
        
    	MPI_Send(buf, position, MPI_PACKED, 1, 99, MPI_COMM_WORLD); 
	}
	else if (myid == 1){ 
		MPI_Recv(buf, 100, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
		
		position = 0 ;
		i = j = 0;
		
		// your code here
		MPI_Unpack(buf,100,&position,&i,1,MPI_INT,MPI_COMM_WORLD);
        MPI_Unpack(buf,100,&position,&j,1,MPI_INT,MPI_COMM_WORLD);
		// end of your code
		
		printf("The number is %d and %d", i, j);
	}

	MPI_Finalize();
	return 0;
}

