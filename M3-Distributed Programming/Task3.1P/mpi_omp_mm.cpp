/* This code is made by Anneshu Nag, Student ID: 2210994760 */ 
#include <iostream> 
#include <vector> 
#include <chrono> 
#include <mpi.h> 
#include <omp.h> 
using namespace std; 
 
void mpiOpenMPMatrixMultiplication(const vector<vector<int>> &A, const vector<vector<int>> &B, 
vector<vector<int>> &C, int rank, int size) 
{ 
    int n = A.size(); 
    int p = size; 
    int rows = n / p; 
    int start = rank * rows; 
    int end = (rank == p - 1) ? n : start + rows; 
 
    #pragma omp parallel for 
    for (int i = start; i < end; i++) 
    { 
        for (int j = 0; j < n; j++) 
        { 
            int sum = 0; 
            for (int k = 0; k < n; k++) 
            { 
                sum += A[i][k] * B[k][j]; 
            } 
            C[i][j] = sum; 
        } 
    } 
} 
int main(int argc, char **argv) 
{ 
    int rank, size; 
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
 
    const int n = 1000; 
    vector<vector<int>> A(n, vector<int>(n)); 
    vector<vector<int>> B(n, vector<int>(n)); 
    vector<vector<int>> C(n, vector<int>(n)); 
 
    // Initialize input matrices A and B 
    srand(time(nullptr)); 
    for (int i = 0; i < n; i++) 
    { 
        for (int j = 0; j < n; j++) 
        { 
            A[i][j] = rand() % 10; 
            B[i][j] = rand() % 10; 
        } 
    } 
    if (rank == 0) 
    { 
        auto start = chrono::steady_clock::now(); 
        mpiOpenMPMatrixMultiplication(A, B, C, rank, size); 
        auto end = chrono::steady_clock::now(); 
 
        cout << "MPI+OpenMP time: " << chrono::duration_cast<chrono::duration<double>>(end - 
start).count() << " seconds" << endl; 
    } 
    MPI_Finalize(); 
    return 0; 
}
