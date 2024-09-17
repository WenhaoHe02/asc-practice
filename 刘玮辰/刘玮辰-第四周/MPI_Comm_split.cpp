#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv)
{
	int myid, numprocs;
	MPI_Comm row_comm, column_comm;
	int myrow, mycolumn;
	int color = 3 ;
		
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	myrow = myid / color ;
	mycolumn = myid % color ;

	// your code here
	MPI_Comm_split(MPI_COMM_WORLD,myrow,mycolumn,&row_comm);
    MPI_Comm_split(MPI_COMM_WORLD,myrow,mycolumn,&column_comm);
    // end of your code
	
	int rowsum, columnsum;
	
	rowsum = myrow;
	columnsum = mycolumn;
	
	MPI_Allreduce(MPI_IN_PLACE, &rowsum, 1, MPI_INT, MPI_SUM, row_comm);  
    MPI_Allreduce(MPI_IN_PLACE, &columnsum, 1, MPI_INT, MPI_SUM, column_comm); 
    
    printf("I'm process %d, my coordinates are (%d, %d), row sum is %d, column sum is %d\n", myid, myrow, mycolumn,  
rowsum, columnsum); 
    
	MPI_Finalize();
	return 0;
} 

