/* This code is made by Anneshu Nag, Student ID: 2210994760 */
/*              MPI based Quick Sort Algorithm              */

#include <iostream>
#include <chrono>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

// Function to perform quick sort
void quick_sort(int *array, int left, int right) {
    int i, j, pivot, tempVal;
    if (left < right) {
        pivot = left;
        i = left;
        j = right;

        while (i < j) {
            // Move the left pointer until finding an element greater than pivot
            while (array[i] <= array[pivot] && i <= right)
                i++;
            // Move the right pointer until finding an element smaller than pivot
            while (array[j] > array[pivot])
                j--;
            if (i < j) {
                // Swap elements at positions i and j
                tempVal = array[i];
                array[i] = array[j];
                array[j] = tempVal;
            }
        }
        // Swap pivot with the element at position j
        tempVal = array[pivot];
        array[pivot] = array[j];
        array[j] = tempVal;

        // Recursively apply quicksort to the subarrays
        quick_sort(array, left, j - 1);
        quick_sort(array, j + 1, right);
    }
}

int main(int argc, char **argv) {
    int rank, size, *array, *chunk, a, x, order;
    MPI_Status status;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Prompt user for input
        cout << "Enter the number of elements in the array: ";
        cin >> a;

        // Allocate memory for the array
        array = new int[a];
    
        // Generate random values for the array
        for (x = 0; x < a; x++)
            array[x] = rand() % 100; // Random numbers between 0 and 99

        // Perform quicksort on the entire array
        quick_sort(array, 0, a - 1);
    }

    // Broadcast the size of the array to all processes
    MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the chunk to be processed by each process
    chunk = new int[a / size];

    // Scatter the array to different processes
    MPI_Scatter(array, a / size, MPI_INT, chunk, a / size, MPI_INT, 0, MPI_COMM_WORLD);

    // Measure the start time of the parallel quicksort
    auto start_time = high_resolution_clock::now();

    // Perform quicksort on the chunk
    quick_sort(chunk, 0, a / size - 1);

    // Measure the end time of the parallel quicksort
    auto end_time = high_resolution_clock::now();

    // Calculate the duration of the parallel quicksort
    auto duration = duration_cast<microseconds>(end_time - start_time);

    if (rank == 0) {
        // Print the execution time of the parallel quicksort
        cout << "Execution time is " << duration.count() << " microseconds" << endl;
    }

    // Merge sorted chunks in a tree-like fashion
    for (order = 1; order < size; order *= 2) {
        if (rank % (2 * order) != 0) {
            // Send chunk to the process with rank 'rank - order'
            MPI_Send(chunk, a / size, MPI_INT, rank - order, 0, MPI_COMM_WORLD);
            break;
        }
        int recv_size = a / size;
        if ((rank + order) < size)
            recv_size = a / size;
        else
            recv_size = a - (rank + order) * (a / size);

        // Receive chunk from the process with rank 'rank + order'
        int *other = new int[recv_size];
        MPI_Recv(other, recv_size, MPI_INT, rank + order, 0, MPI_COMM_WORLD, &status);

        // Merge chunks
        int *temp = new int[a / size + recv_size];
        int i = 0, j = 0, k = 0;
        while (i < a / size && j < recv_size) {
            if (chunk[i] < other[j])
                temp[k++] = chunk[i++];
            else
                temp[k++] = other[j++];
        }
        while (i < a / size)
            temp[k++] = chunk[i++];
        while (j < recv_size)
            temp[k++] = other[j++];

        // Update 'chunk' with the merged chunk
        delete[] other;
        delete[] chunk;
        chunk = temp;
    }

    if (rank == 0) {
        // Allocate memory for the final sorted array
        array = new int[a];
    }

    // Gather sorted chunks from all processes
    MPI_Gather(chunk, a / size, MPI_INT, array, a / size, MPI_INT, 0, MPI_COMM_WORLD);

    // if (rank == 0) {
    //     // Print the final sorted array
    //     for (x = 0; x < a; x++) {
    //         cout << array[x] << " ";
    //     }
    //     cout << endl;
    // }

    // Finalize MPI environment
    MPI_Finalize();
    return 0;
}
