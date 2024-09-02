#include<iostream>
#include<mpi.h>
using namespace std;
int main(int argc,char** argv)
{
    int size=0;
    int rank=0;
    int a[6]={9,19,29,39,49,59};
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int data=0;
    double startTime = MPI_Wtime();
    MPI_Scatter(a,1,MPI_INT,&data,1,MPI_INT,0,MPI_COMM_WORLD);
    printf("%d, %d\n",rank,data);
    double endTime=MPI_Wtime();
    printf("TIME: %d, %lf\nTICK: %.10lf\n\n",rank,endTime-startTime,MPI_Wtick());
    MPI_Finalize();
    return 0;
}
