/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*     This is a OMP parallel matrix multiplication.      */

#include <iostream>
#include <fstream>
#include <random>
#include <omp.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

#define matrixSize 1000   // Change to compute for different matrix sizes

int matrixA[matrixSize][matrixSize];
int matrixB[matrixSize][matrixSize];
int matrixOut[matrixSize][matrixSize];

int main() 
{
    cout << "Running Matrix Multiplication for size: " << matrixSize << endl;

    int i,j,k;
    omp_set_num_threads(omp_get_num_procs());
    // Generating a random matrixSizeber
    random_device random;
    mt19937 genRand(random());
    uniform_int_distribution<> dist(0, 100);
   
    // initializing matrices A and B with random values
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            matrixA[i][j] = dist(genRand);
            matrixB[i][j] = dist(genRand);
        }
    }
    // Starting the timer
    auto start = high_resolution_clock::now();
    
    #pragma omp parallel for private(i, j, k) shared(matrixA, matrixB, matrixOut)
    for (i = 0; i < matrixSize; ++i) {
        for (j = 0; j < matrixSize; ++j) {
            matrixOut[i][j] = 0;
            for (k = 0; k < matrixSize; ++k) {
                matrixOut[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    // Stopping timer
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Printing output time
    cout << "Time taken for OpenMP matrix multiplication: " << duration.count() << " microseconds"<< endl;

    // Open a text file for writing
    ofstream outputFile("omp_output.txt");

    if (outputFile.is_open()) {
        // Redirect the output to the file
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(outputFile.rdbuf());

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                cout << matrixOut[i][j] << " ";
            }
            cout << endl;
        }

        // Close the file
        outputFile.close();

        // Restore cout back to the console
        cout.rdbuf(coutbuf);

        cout << "Output has been written to 'omp_output.txt'" << endl;
    } else {
        cout << "Unable to open output.txt for writing." << endl;
    }
}