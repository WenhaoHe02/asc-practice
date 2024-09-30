#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int buffer[100]; // 用于存储打包后的数据
    int position = 0; // 当前位置指针
    int i;

    // 初始化 MPI 环境
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // 进程 0 打包数据
        int data[] = {1, 2, 3};
        int count = sizeof(data) / sizeof(data[0]);

        // 使用 MPI_Pack 将数据打包到 buffer
        MPI_Pack(data, count, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);

        // 发送打包后的数据到进程 1
        MPI_Send(buffer, position, MPI_PACKED, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        // 进程 1 接收打包后的数据
        MPI_Status status;
        MPI_Recv(buffer, 100, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status);

        // 解析接收到的数据
        int received_data[3];
        int received_count = 3;
        position = 0;
        MPI_Unpack(buffer, 100, &position, received_data, received_count, MPI_INT, MPI_COMM_WORLD);

        // 打印接收到的数据
        for (i = 0; i < received_count; i++) {
            printf("Process %d received: %d\n", rank, received_data[i]);
        }
    }

    // 清理 MPI 环境
    MPI_Finalize();
    return 0;
}
