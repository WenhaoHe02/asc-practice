#include<iostream>
#include<mpi.h>
using namespace std;
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
    MPI_Group_excl( first , 2, seq ,&second);
    int curRank=-1;
    MPI_Group_rank( second , &curRank);
    printf("%d,%d\n",rank,curRank);
    MPI_Group_free( &first);
    MPI_Group_free( &second);
    MPI_Finalize();
    return 0;
}
