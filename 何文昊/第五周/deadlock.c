#include<mpi.h>
#include<stdio.h>
#include<string.h>
int main(int argc, char** argv) {
    int rank, size;
    const char* str = "no deadlock";
    char str_rec[100]; 
    MPI_Init( &argc , &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Sendrecv(str, strlen(str) + 1, MPI_CHAR , (rank + 1) % size , rank , str_rec , 100 , MPI_CHAR , (rank - 1) % size , rank , MPI_COMM_WORLD , status);
    MPI_Finalize();
    return 0;
}