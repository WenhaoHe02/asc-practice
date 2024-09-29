// #include<mpi.h>
// #include<stdio.h>
// #include<string.h>
// int main(int argc, char** argv) {
//     MPI_Init(&argc, &argv);
//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);
//     if (size < 2) {
//         printf("至少需要两个进程");
//     }
//     if (rank == 0) {
//         const char* str = "Hello, process1";
//         MPI_Send( str , strlen(str) + 1 , MPI_CHAR , 1 , 0 , MPI_COMM_WORLD);
//         printf("进程0发送了字符串%s\n", str);
//     }
//     if (rank == 1) {
//         char buffer[100]; 
//         MPI_Recv( buffer , 100, MPI_CHAR , 0 , 0 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         printf("进程1接收了字符串%s\n", buffer);
//     }
//     MPI_Finalize();
// }
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int numtasks, rank, dest, source, rc, count, tag = 1;
    char inmsg, outmsg = 'x';  
    MPI_Status Stat;  

    MPI_Init(&argc, &argv);  
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  

    
    if (rank == 0) {
        dest = 1;  
        source = 1; 
        MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);  
        //MPI_Isend
        MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);  
    }
    // 任务1等待来自任务0的消息，并返回一条消息
    //消息的tag必须是匹配的
    else if (rank == 1) {
        dest = 0; 
        source = 0;  
        MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);  
        MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD); 
    }

    MPI_Get_count(&Stat, MPI_CHAR, &count);  
    printf("Task %d: Received %d char(s) from task %d with tag %d\n", rank, count, Stat.MPI_SOURCE, Stat.MPI_TAG);

    MPI_Finalize(); 
    return 0;
}
