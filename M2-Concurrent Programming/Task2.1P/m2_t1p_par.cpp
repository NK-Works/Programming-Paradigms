/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*        This is a parallel matrix multiplication.     */
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <thread>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 2000
#define NUM_THREADS thread::hardware_concurrency() // Fetching number of cores in the system

int matrix1[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
int matrix2[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
int matrixOut[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];

struct thread_data
{
    int thread_id;
};

void *multiply_matrices(void *threadarg)
{
    struct thread_data *my_data;
    my_data = (struct thread_data *)threadarg;
    int start_row = my_data->thread_id * (MAX_MATRIX_SIZE / NUM_THREADS);
    int end_row = start_row + (MAX_MATRIX_SIZE / NUM_THREADS);
    
    for (int i = start_row; i < end_row; i++)
    {
        for (int j = 0; j < MAX_MATRIX_SIZE; j++)
        {
            int sum = 0;
            for (int k = 0; k < MAX_MATRIX_SIZE; k++)
            {
                sum += matrix1[i][k] * matrix2[k][j];
            }
            matrixOut[i][j] = sum;
        }
    }
    pthread_exit(nullptr);
    return nullptr;
}

int main()
{
    int matrixSize;

    // Getting the size of the matrix from the user and ensure it is between 2 and 10
    do
    {
        cout << "Enter the size of the matrix (between " << MIN_MATRIX_SIZE << " and " << MAX_MATRIX_SIZE << "): ";
        cin >> matrixSize;

        if (matrixSize < MIN_MATRIX_SIZE || matrixSize > MAX_MATRIX_SIZE)
        {
            cout << "Invalid size. Please enter a size between " << MIN_MATRIX_SIZE << " and " << MAX_MATRIX_SIZE << "." << endl;
        }

    } while (matrixSize < MIN_MATRIX_SIZE || matrixSize > MAX_MATRIX_SIZE);
    
    pthread_t threads[NUM_THREADS];
    struct thread_data thread_data_array[NUM_THREADS];
    
    // Generating a random matrixSizeber
    random_device random;
    mt19937 genRand(random());
    uniform_int_distribution<> dist(0, 100);

     // Initializing the matrices
    for (int i = 0; i < matrixSize; i++)
    {
        for (int j = 0; j < matrixSize; j++)
        {
            matrix1[i][j] = dist(genRand);
            matrix2[i][j] = dist(genRand);
        }
    }
    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Creating threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_data_array[i].thread_id = i;
        pthread_create(&threads[i], nullptr, multiply_matrices, (void *)&thread_data_array[i]);
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

        cout << "\nOutput has been written to 'par_output.txt'" << endl;
    } else {
        cerr << "\nUnable to open output.txt for writing." << endl;
    }
    pthread_exit(nullptr);

}