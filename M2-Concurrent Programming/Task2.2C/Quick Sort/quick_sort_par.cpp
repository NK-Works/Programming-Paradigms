/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*    This is a pthread parallel quick sort algorithm.    */
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS 4

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

// Function to perform quicksort using multiple threads
void performQuickSort(vector<int> &inputArray, int left, int right, int numThreads)
{
    int pivotIndex;
    if (left < right)
    {
        pivotIndex = partition(inputArray, left, right);
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

// Final merger of the processes that have been sorted individually 
void final_merge(vector<int>& data, int start, int end) {
  if (start < end) {
    int mid = start + (end - start) / 2;

    final_merge(data, start, mid);
    final_merge(data, mid + 1, end);

    int n1 = mid - start + 1;
    int n2 = end - mid;
    vector<int> left_temp(n1);
    vector<int> right_temp(n2);

    for (int i = 0; i < n1; i++)
      left_temp[i] = data[start + i];
    for (int j = 0; j < n2; j++)
      right_temp[j] = data[mid + 1 + j];

    // Merge the temporary arrays back into data
    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
      if (left_temp[i] <= right_temp[j]) {
        data[k] = left_temp[i];
        i++;
      } else {
        data[k] = right_temp[j];
        j++;
      }
      k++;
    }
    while (i < n1) {
      data[k] = left_temp[i];
      i++;
      k++;
    }

    while (j < n2) {
      data[k] = right_temp[j];
      j++;
      k++;
    }
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
    performQuickSort(inputArray, 0, inputSize - 1, NUM_THREADS);
    final_merge(inputArray, 0, inputSize - 1);
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
