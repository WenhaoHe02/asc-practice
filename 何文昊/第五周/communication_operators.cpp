#include <mpi.h>
#include <vector>
#include <iostream>
int main(int argc, char **argv)
{
    int rank;
    int size = 0;
    int buff = 0;
    constexpr int root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int arr[2]{rank + 1, rank + 2};
    int rec_arr[2]{};
    int gather[1] = {rank};
    int *rec_gather = new int[size];
    int *scatter = new int[size];
    for (int i = 0; i < size; i++)
    {
        scatter[i] = i;
    }
    int rec_scatter[1]{0};
    for (int i = 0; i < size; i++)
    {
        rec_gather[i] = 0;
    }
    if (rank == root)
    {
        buff = 1;
    }
    MPI_Bcast(&buff, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Reduce(arr, rec_arr, 2, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
    MPI_Allreduce(arr, rec_arr, 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Scan(arr, rec_arr, 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Gather(gather, 1, MPI_INT, rec_gather, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatter(scatter, 1, MPI_INT, rec_scatter, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Finalize();
    std::ios::sync_with_stdio(false);
    std::cout << "rank = " << rank << " buff = " << buff << '\n';
    for (int i = 0; i < 2; i++)
    {
        std::cout << rec_arr[i] << '\n';
    }
    if (rank == 0)
    {
        std::cout << "gather: ";
        for (int i = 0; i < size; i++)
        {
            std::cout << rec_gather[i] << " ";
        }
        std::cout << "\n";
    }
    delete[] rec_gather;
    std::cout << "scatter: " << rec_scatter[0] << "\n";
    return 0;
}