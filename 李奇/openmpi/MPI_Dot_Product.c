#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            printf("Usage: %s N\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    FILE *file1 = fopen("vec1.txt", "r");
    FILE *file2 = fopen("vec2.txt", "r");
    if (file1 == NULL) {
        printf("Error opening vec1.txt\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (file2 == NULL) {
        printf("Error opening vec2.txt\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int N = atoi(argv[1]);
    int local_N = N / size;
    int remainder = N % size;
    int start = rank * local_N + (rank < remainder ? rank : remainder);
    int end = start + local_N + (rank < remainder ? 1 : 0);

    double *vec1_local = (double *)malloc((end - start) * sizeof(double));
    double *vec2_local = (double *)malloc((end - start) * sizeof(double));

    fseek(file1, start * sizeof(double), SEEK_SET);
    fseek(file2, start * sizeof(double), SEEK_SET);
    for (int i = 0; i < end - start; i++) {
        fscanf(file1, "%lf", &vec1_local[i]);
        fscanf(file2, "%lf", &vec2_local[i]);
    }

    fclose(file1);
    fclose(file2);

    double local_dot_product = 0.0;
    for (int i = 0; i < end - start; i++) {
        local_dot_product += vec1_local[i] * vec2_local[i];
    }

    MPI_Win win;
    double *shared_dot_product;

    MPI_Aint size_shared = (rank == 0) ? sizeof(double) : 0; 
    MPI_Win_allocate_shared(size_shared, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &shared_dot_product, &win);

    if (rank != 0) {
        int disp_unit;
        MPI_Aint win_size;
        MPI_Win_shared_query(win, 0, &win_size, &disp_unit, &shared_dot_product);
    }

    // Synchronize before updating shared memory
    MPI_Win_fence(0, win);

    if (rank == 0) {
        *shared_dot_product = local_dot_product;
    } else {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        *shared_dot_product += local_dot_product;
        MPI_Win_unlock(0, win);
    }

    // Synchronize after all processes have updated the shared memory
    MPI_Win_fence(0, win);

    if (rank == 0) {
        printf("Dot product result: %lf\n", *shared_dot_product);
    }

    MPI_Win_free(&win);
    free(vec1_local);
    free(vec2_local);

    MPI_Finalize();
    return 0;
}
