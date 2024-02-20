/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*     This is a OPM parallel matrix multiplication.      */

#include <iostream>
#include <fstream>
#include <random>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

int main() {
    int matrixSize;

    // Getting the size of the matrix from the user and ensure it is between 2 and 2000
    do {
        cout << "Enter the size of the matrix (between 2 and 2000): ";
        cin >> matrixSize;

        if (matrixSize < 2 || matrixSize > 2000) {
            cout << "Invalid size. Please enter a size between 2 and 2000." << endl;
        }

    } while (matrixSize < 2 || matrixSize > 2000);

    int matrixA[matrixSize][matrixSize], matrixB[matrixSize][matrixSize], matrixOut[matrixSize][matrixSize]; // A,B,C matrix

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

    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Performing matrix multiplication in parallel
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            matrixOut[i][j] = 0;
            for (int k = 0; k < matrixSize; ++k) {
                matrixOut[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    // Stop the timer
    auto end_time = high_resolution_clock::now();

    // Calculate and print the elapsed time
    auto duration = duration_cast<microseconds>(end_time - start_time);
    cout << "Time taken for matrix multiplication: " << duration.count() << " microseconds." << endl;

    // Open a text file for writing
    ofstream outputFile("opm_output.txt");

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

        cout << "\nOutput has been written to 'opm_output.txt'" << endl;
    } else {
        cerr << "\nUnable to open output.txt for writing." << endl;
    }

    return 0;
}
