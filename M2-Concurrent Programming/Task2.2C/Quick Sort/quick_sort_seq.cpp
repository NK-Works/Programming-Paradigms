/* This code is made by Anneshu Nag, Student ID-2210994760*/
/*        This is a sequential quick sort algorithm.      */
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

int partition(vector<int>& arrayValue, int left, int right) {
    int pivotIdx = (left + right) / 2;
    int pivotValue = arrayValue[pivotIdx];

    swap(arrayValue[pivotIdx], arrayValue[right]);

    int storeTdx = left;
    for(int i = left; i < right; i++) {
        if(arrayValue[i] < pivotValue) {
            swap(arrayValue[i], arrayValue[storeTdx]);
            storeTdx++;
        }
    }
    swap(arrayValue[storeTdx], arrayValue[right]);

    return storeTdx;
}

void quickSort(vector<int>& arrayValue, int left, int right) {
    if(left < right) {
        int pivotIdx = partition(arrayValue, left, right);
        quickSort(arrayValue, left, pivotIdx - 1);
        quickSort(arrayValue, pivotIdx + 1, right);
    }
}

int main() {
    vector<int> arrayValue;
    int inputSize;
    cout << "Enter the size of the array you want to sort: ";
    cin >> inputSize;
    for(int i = 0; i < inputSize; i++) {
        arrayValue.push_back(rand() % 100);
    }
    int n = arrayValue.size();
    auto start = high_resolution_clock::now();

    quickSort(arrayValue, 0, n - 1);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Execution Time: " << duration.count() << " microseconds." << endl;
    return 0;
}