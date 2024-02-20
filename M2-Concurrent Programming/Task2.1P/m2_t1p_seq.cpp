/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*        This is a sequential matrix multiplication.     */
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

int main()
{
    int matrixSize;

    // Getting the size of the matrix from the user and ensure it is between 2 and 12
    do
    {
        cout << "Enter the size of the matrix (between 2 and 12): ";
        cin >> matrixSize;

        if (matrixSize < 2 || matrixSize > 12)
        {
            cout << "Invalid size. Please enter a size between 2 and 12." << endl;
        }

    } while (matrixSize < 2 || matrixSize > 12);

    // Initialise the matrices for the inputs and output
    int matrixA[matrixSize][matrixSize], matrixB[matrixSize][matrixSize], matrixOut[matrixSize][matrixSize]; 

    // Generating a random numbers for the input matrices
    random_device random;
    mt19937 genRand(random());
    uniform_int_distribution<> dist(0, 100);

    // Initializing matrices A and B with random values
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            matrixA[i][j] = dist(genRand);
            matrixB[i][j] = dist(genRand);
        }
    }

    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Performing matrix multiplication
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            matrixOut[i][j] = 0;
            for (int k = 0; k < matrixSize; ++k)
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

    // Printing the result to the console
    cout << "::: Resulting Matrix :::" << endl;
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            cout << matrixOut[i][j] << " ";
        }
        cout << endl;
    }
    // Open a text file for writing
    ofstream outputFile("seq_output.txt");

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

        cout << "\nOutput has been written to 'seq_output.txt'" << endl;
    } else {
        cerr << "\nUnable to open output.txt for writing." << endl;
    }

    return 0;
}
