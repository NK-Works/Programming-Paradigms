/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*        This is a sequential matrix multiplication.     */
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

#define MATRIX_SIZE 1000
int main()
{
    // Initialize the matrices for the inputs and output using dynamic memory allocation
    int** matrixA = new int*[MATRIX_SIZE];
    int** matrixB = new int*[MATRIX_SIZE];
    int** matrixOut = new int*[MATRIX_SIZE];

    for (int i = 0; i < MATRIX_SIZE; ++i)
    {
        matrixA[i] = new int[MATRIX_SIZE];
        matrixB[i] = new int[MATRIX_SIZE];
        matrixOut[i] = new int[MATRIX_SIZE];
    }

    // Generating a random matrixSizeber
    random_device random;
    mt19937 genRand(random());
    uniform_int_distribution<> dist(0, 100);

    // Initializing the matrices
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            matrixA[i][j] = dist(genRand);
            matrixB[i][j] = dist(genRand);
        }
    }
    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Performing matrix multiplication
    for (int i = 0; i < MATRIX_SIZE; ++i)
    {
        for (int j = 0; j < MATRIX_SIZE; ++j)
        {
            matrixOut[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; ++k)
            {
                matrixOut[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    // Stop the timer
    auto end_time = high_resolution_clock::now();
    // Calculate and print the elapsed time
    auto duration = duration_cast<microseconds>(end_time - start_time);
    cout << "Time taken for matrix multiplication: " << duration.count() << " microseconds." << endl;

    // Deallocate memory when done
    for (int i = 0; i < MATRIX_SIZE; ++i)
    {
        delete[] matrixA[i];
        delete[] matrixB[i];
        delete[] matrixOut[i];
    }

    delete[] matrixA;
    delete[] matrixB;
    delete[] matrixOut;
    return 0;
}
