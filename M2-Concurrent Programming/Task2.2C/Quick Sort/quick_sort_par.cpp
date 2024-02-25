/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*    This is a pthread parallel quick sort algorithm.    */
#include <iostream>
#include <vector>
#include <chrono>
#include <pthread.h>
#include <thread>

using namespace std;
using namespace std::chrono;

// Define the number of threads based on the hardware concurrency
#define NUM_THREADS thread::hardware_concurrency()

// Structure to hold arguments for each thread
struct ThreadArgs
{
    vector<int> *inputArray;  // Pointer to the input array
    int left;                 
    int right;
};

// Partition function for the quicksort algorithm
int partition(vector<int> &inputArray, int left, int right)
{
    int pivot = inputArray[right];
    int i = left - 1;
    for (int j = left; j < right; j++)
    {
        if (inputArray[j] < pivot)
        {
            i++;
            swap(inputArray[i], inputArray[j]);
        }
    }
    swap(inputArray[i + 1], inputArray[right]);
    return (i + 1);
}

// Recursive quicksort function
void quickSort(vector<int> &inputArray, int left, int right)
{
    if (left < right)
    {
        int pivotIndex = partition(inputArray, left, right);
        quickSort(inputArray, left, pivotIndex - 1);
        quickSort(inputArray, pivotIndex + 1, right);
    }
}

// Thread helper function for parallel quicksort
void *quickSortHelpThread(void *arg)
{
    ThreadArgs *info = static_cast<ThreadArgs *>(arg);
    quickSort(*(info->inputArray), info->left, info->right);
    return nullptr;
}

// Function to perform quicksort using multiple threads
void performQuickSort(vector<int> &inputArray, int left, int right, int numThreads)
{
    if (left < right)
    {
        int pivotIndex = partition(inputArray, left, right);

        // Create an array to store thread IDs and an array of ThreadArgs structures
        pthread_t threads[numThreads];
        ThreadArgs threadArgs[numThreads];
        // Calculate the range each thread will handle
        int range = (right - left + 1) / numThreads;
        // Create threads and pass the subarray information
        for (int i = 0; i < numThreads; ++i)
        {
            threadArgs[i] = {&inputArray, left + i * range, left + (i + 1) * range - 1};
            pthread_create(&threads[i], nullptr, quickSortHelpThread, &threadArgs[i]);
        }
        // Wait for all threads to finish
        for (int i = 0; i < numThreads; ++i)
        {
            pthread_join(threads[i], nullptr);
        }
    }
}

int main()
{
    vector<int> inputArray;
    int inputSize;

    // Input array size from the user
    cout << "Enter the size of the array you want to sort: ";
    cin >> inputSize;

    // Fill the array with random values
    for (int i = 0; i < inputSize; i++)
    {
        inputArray.push_back(rand() % 100);
    }

    // Record the start time
    auto start = high_resolution_clock::now();
    // Call the parallel quicksort function
    performQuickSort(inputArray, 0, inputArray.size() - 1, NUM_THREADS);
    // Record the stop time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print the execution time
    cout << "Execution Time: " << duration.count() << " microseconds." << endl;
    return 0;
}
