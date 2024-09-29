#include<mpi.h>
#include<ctime>
#include<iostream>
#include<algorithm>
void compare_split(int local_n, int* other, int* local, int* wspace) {
// 所有数组皆为有序的
int p1 = 0, p2 = 0;
int local_i = 0;
while (p1 < local_n && p2 < local_n && local_i < local_n) {
if (other[p1] < wspace[p2]) {
    local[local_i] = other[p1]; 
    p1++;
    local_i++;
} else if (other[p1] < wspace[p2]) {
    local[local_i] = wspace[p2];
    p2++;
    local_i++;
} else {
     local[local_i] = wspace[p2];
    p2++;
    p1++;
    local_i++;
}
}
}
int main(int argc, char*argv[]) {
    srand(static_cast<unsigned int>(time(0)));
    int rank, size;
    int* local, *other, *wspac, oddfriend, evenfriend;
    int n = 80;
    int proc = 8;
    int local_n = n / proc;
    local = new int[local_n];
    wspac = new int[local_n];
    for (int i = 0; i < local_n; i++) {
        local[i] = rand() % 100;
    }
    std::sort(local,local + local_n);
     for (int i = 0; i < local_n; i++) {
        wspac[i] = local[i];
    }
    other = new int[local_n];
    MPI_Init(&argc, &argv);
    MPI_Status* status;
    MPI_Comm_rank(MPI_COMM_WORLD , &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if ((rank + 1) % 2 == 0) {
        oddfriend = rank - 1;
        evenfriend = rank + 1;
    } else {
        oddfriend = rank + 1;
        evenfriend = rank - 1;
    }
    if (oddfriend == -1) {
        oddfriend = MPI_PROC_NULL;
    }
    if (evenfriend == -1) {
        evenfriend = MPI_PROC_NULL;
    }
    for (int i = 0; i < size; i++) {
        if ((i + 1) % 2 != 0) {
            MPI_Sendrecv(local, local_n, MPI_INT, oddfriend, 1, other, local_n, MPI_INT, evenfriend, 1, MPI_COMM_WORLD, status);
        } else {
            MPI_Sendrecv(local, local_n, MPI_INT, evenfriend, 1, other, local_n, MPI_INT, evenfriend, 1, MPI_COMM_WORLD, status);
        }
        compare_split(local_n, other, local, wspac);
    }
    MPI_Finalize();
    delete[] local;
    delete[] other;
    delete[] wspac;
    
    return 0;
}