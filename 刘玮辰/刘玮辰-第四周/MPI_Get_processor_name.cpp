#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int len;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Init(&argc, &argv);

	//your code here
    MPI_Get_processor_name( name , &len);
	//end of your code

	printf("Hello, world. I am %s.\n", name);

	MPI_Finalize();
	return 0;
}

