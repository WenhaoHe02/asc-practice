#include<cstdio>
#include<iostream>
#include<mpi.h>
using namespace std;

int main(int argc, char **argv)
{
    int size=0;
    int rank=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int a[10];
    if(rank==1)
    {
        for(int i=0;i<10;i++) a[i]=i; 
    }
    MPI_Bcast(a,10,MPI_INT,1,MPI_COMM_WORLD); //it needs to be put outside "IF" but i donno why
    for(int i=0;i<10;i++)
    {
        printf("%d ",a[i]);
    }
    printf(": %d\n",rank);
    MPI_Finalize();
    return 0;
}
