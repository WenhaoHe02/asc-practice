#include<mpi.h>
#include<cstdio>
int main(int argc,char** argv)
{
    int size=0;
    int rank=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int a[6]={9,9,9,9,9,9};
    int data=rank;
    MPI_Gather(&data,1,MPI_INT,a,1,MPI_INT,0,MPI_COMM_WORLD);
    printf("%d\n",rank);
    if(rank==0)
    {
        for(int i=0;i<size;i++)
        {
            printf("%d ",a[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}
