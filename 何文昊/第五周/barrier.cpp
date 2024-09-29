#include <mpi.h>
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout << "Process " << rank << " before Barrier." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(100 * rank));
    MPI_Barrier(MPI_COMM_WORLD);

    std::cout << "Process " << rank << " after Barrier." << std::endl;

    MPI_Finalize();
    return 0;
}
