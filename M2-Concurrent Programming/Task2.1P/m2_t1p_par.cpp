/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*   This is a pthread parallel matrix multiplication.    */
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <random>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

#define NUM_THREADS thread::hardware_concurrency() // Fetching number of cores in the system

int matrixSize;
// Initialize the matrices for the inputs and output using dynamic memory allocation
int** matrixA;
int** matrixB;
int** matrixOut;

void *multiply_matrices(void *threadarg)
{
    int threadNum = *((int *)threadarg);
    int threadSize = matrixSize / NUM_THREADS;
    int startRow = threadNum * threadSize;
    int endRow = (threadNum == NUM_THREADS - 1) ? matrixSize : (threadNum + 1) * threadSize;

    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            int sum = 0;
            for (int k = 0; k < matrixSize; k++)
            {
                sum += matrixA[i][k] * matrixB[k][j];
            }
            matrixOut[i][j] = sum;
        }
    }
    return NULL;
}

int main()
{
    // Getting the size of the matrix from the user and ensure it is between 2 and 2000
    do
    {
        cout << "Enter the size of the matrix (between " << 2 << " and " << 2000 << "): ";
        cin >> matrixSize;

        if (matrixSize < 2 || matrixSize > 2000)
        {
            cout << "Invalid size. Please enter a size between " << 2 << " and " << 2000 << "." << endl;
        }

    } while (matrixSize < 2 || matrixSize > 2000);

    // Initialize the matrices for the inputs and output using dynamic memory allocation
    matrixA = new int*[matrixSize];
    matrixB = new int*[matrixSize];
    matrixOut = new int*[matrixSize];

    for (int i = 0; i < matrixSize; ++i)
    {
        matrixA[i] = new int[matrixSize];
        matrixB[i] = new int[matrixSize];
        matrixOut[i] = new int[matrixSize];
    }

    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    // Generating a random matrixSizeber
    random_device random;
    mt19937 genRand(random());
    uniform_int_distribution<> dist(0, 100);

    // Initializing the matrices
    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            matrixA[i][j] = dist(genRand);
            matrixB[i][j] = dist(genRand);
        }
    }

    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Creating threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, multiply_matrices, (void*)&thread_args[i]);
    }

    // Waiting for threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], nullptr);
    }

    // Stop the timer
    auto end_time = high_resolution_clock::now();
    // Calculate and print the elapsed time
    auto duration = duration_cast<microseconds>(end_time - start_time);
    cout << "Time taken for matrix multiplication: " << duration.count() << " microseconds." << endl;

    // Open a text file for writing
    ofstream outputFile("par_output.txt");

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

        cout << "Output has been written to 'par_output.txt'" << endl;
    } else {
        cerr << "Unable to open output.txt for writing." << endl;
    }
    // Deallocate memory when done
    for (int i = 0; i < matrixSize; ++i)
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
