/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*    This is a pthread parallel quick sort algorithm.    */

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

// Structure to hold arguments for each thread
struct ThreadArgs
{
    vector<int> *inputArray;
    int left;
    int right;
};

// Partition function for the quicksort algorithm
int partition(vector<int> &inputArray, int left, int right)
{
    int pivotIdx = (left + right) / 2;
    int pivotValue = inputArray[pivotIdx];

    swap(inputArray[pivotIdx], inputArray[right]);

    int storeTdx = left;
    for (int i = left; i < right; i++)
    {
        if (inputArray[i] < pivotValue)
        {
            swap(inputArray[i], inputArray[storeTdx]);
            storeTdx++;
        }
    }
    swap(inputArray[storeTdx], inputArray[right]);

    return storeTdx;
}

// Sequential quicksort function
void quickSort(vector<int> &inputArray, int left, int right)
{
    if (left < right)
    {
        int pivotIdx = partition(inputArray, left, right);
        quickSort(inputArray, left, pivotIdx - 1);
        quickSort(inputArray, pivotIdx + 1, right);
    }
}

// Thread helper function for parallel quicksort
void *quickSortHelpThread(void *arg)
{
    ThreadArgs *info = static_cast<ThreadArgs *>(arg);
    quickSort(*(info->inputArray), info->left, info->right);
    return nullptr;
}

// Function to perform parallel quicksort using pthread
void performQuickSort(vector<int> &inputArray, int left, int right)
{
    if (left < right)
    {
        int pivotIdx = partition(inputArray, left, right);

        // Creating threads to sort the partitions
        pthread_t leftThread, rightThread;
        ThreadArgs leftArgs = {&inputArray, left, pivotIdx - 1};
        ThreadArgs rightArgs = {&inputArray, pivotIdx + 1, right};
        pthread_create(&leftThread, nullptr, quickSortHelpThread, &leftArgs);
        pthread_create(&rightThread, nullptr, quickSortHelpThread, &rightArgs);
        
        // Wait for the threads to finish and join
        pthread_join(leftThread, nullptr);
        pthread_join(rightThread, nullptr);
    }
}

int main()
{
    vector<int> inputArray;
    int inputSize;
    cout << "Enter the size of the inputArray you want to sort: ";
    cin >> inputSize;
    for (int i = 0; i < inputSize; i++)
    {
        inputArray.push_back(rand() % 100);
    }

    // // Print input inputArray (For testing purpose) 
    // cout << "Input inputArray:" << endl;
    // for (int i = 0; i < inputSize; i++)
    // {
    //     cout << inputArray[i] << " ";
    // }
    // cout << endl;

    auto start = high_resolution_clock::now();
    performQuickSort(inputArray, 0, inputSize - 1);  // Add numeric in the paameter and uncomment the other one if you wish to test the firt implementation
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Execution Time: " << duration.count() << " microseconds." << endl;

    // // Print sorted inputArray (For testing purpose) 
    // cout << "Sorted inputArray:" << endl;
    // for (int i = 0; i < inputSize; i++)
    // {
    //     cout << inputArray[i] << " ";
    // }
    // cout << endl;

    return 0;
}
