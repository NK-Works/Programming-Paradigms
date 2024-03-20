/* This code is made by Anneshu Nag, Student ID: 2210994760 */
/*          This is MPI based Matrix Multiplication         */
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define N 1000 // Size of the matrices

void generate_matrix(int *matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i * size + j] = rand() % 10; // Random values between 0 and 9
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (N % numprocs != 0) {
        if (rank == 0)
            cout << "Matrix size should be divisible by the number of processes." << endl;
        MPI_Finalize();
        exit(1);
    }

    int local_N = N / numprocs;
    int *A = new int[N * N];
    int *B = new int[N * N];
    int *C = new int[N * N];

    auto start_time = high_resolution_clock::now(); // Start time
    if (rank == 0) {
        generate_matrix(A, N);
        generate_matrix(B, N);
    }

    int *local_A = new int[local_N * N];
    int *local_C = new int[local_N * N];

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    MPI_Scatter(A, local_N * N, MPI_INT, local_A, local_N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                local_C[i * N + j] += local_A[i * N + k] * B[k * N + j];
            }
        }
    }

    MPI_Gather(local_C, local_N * N, MPI_INT, C, local_N * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    if (rank == 0) {
        auto end_time = high_resolution_clock::now(); // End time
        auto duration = duration_cast<microseconds>(end_time - start_time); // Calculate duration
        std::cout << "MPI Execution Time: " << duration.count() << " microseconds" << std::endl;
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] local_A;
    delete[] local_C;

    MPI_Finalize();

    return 0;
}
