#include<iostream>
#include<cstdio>
#include<mpi.h>
using namespace std;
int main(int argc, char** argv)
{
    int size=0;
    int rank=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int data=0, global_sum=0;
        // 每个进程生成一些数据
    for (int i = 0; i < 10; ++i) {
        data += rank * 10 + i; // 生成与进程相关的数据
    }
    // 输出未同步前的数据
    printf("Process %d before barrier: Data = %d\n", rank, data);

    // 执行屏障同步
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&data,&global_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    // 输出同步后的数据
    printf("Process %d after barrier: Data = %d\n", rank, data);
    printf("GLOBAL_SUM: %d\n\n",global_sum);
    MPI_Finalize();
    return 0;
}
