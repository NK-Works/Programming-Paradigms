__kernel void multiply_matrices(const int M, const int N, const int K,
                      const __global int* A,
                      const __global int* B,
                      __global int* C)
{
  const int globalRow = get_global_id(0); // Row ID of C (0..M)
  const int globalCol = get_global_id(1); // Col ID of C (0..N)
  if (globalRow < M && globalCol < N) { // Corrected condition
    int sum = 0;
    for (int i = 0; i < K; ++i) { // Loop bounds corrected to K
      sum += A[globalRow * K + i] * B[i * N + globalCol]; // Adjusted indexing
    }
    C[globalRow * N + globalCol] = sum; // Store result in matrix C
  }
}