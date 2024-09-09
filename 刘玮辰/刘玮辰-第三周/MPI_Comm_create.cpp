#include<stdio.h>
#include<mpi.h>
int main(int argc,char** argv)
{
    int size=0;
    int rank=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Group first,second;
    MPI_Comm_group(MPI_COMM_WORLD,&first);
    int seq[]={1,3};
    MPI_Group_incl( first , 2, seq ,&second);
    MPI_Comm newComm;
    MPI_Comm_create(MPI_COMM_WORLD,second,&newComm);
    int commsize=0;
    if(newComm!=MPI_COMM_NULL) MPI_Comm_size( newComm , &commsize);
    printf("%d\n",commsize);
    MPI_Group_free( &first);
    MPI_Group_free( &second);
    if(MPI_COMM_NULL!=newComm)
        MPI_Comm_free(&newComm);
    MPI_Finalize();
    return 0;
}
