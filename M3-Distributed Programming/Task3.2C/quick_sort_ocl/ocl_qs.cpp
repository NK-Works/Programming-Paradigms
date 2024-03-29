/* This code is made by Anneshu Nag, Student ID: 2210994760 */
/*             MPI + OpenCL Quick Sort Algorithm            */

#include <iostream>
#include <vector>
#include <chrono>
#include <mpi.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

using namespace std;
using namespace std::chrono;

#define array_size 1000000 // Can be changed

// Creating a random array
void create_array(vector<int>& vect, int size) {
 for (int i = 0; i < size; i++) {
  vect.push_back(rand() % 100);
 }
}

// Kernel file for the quick sort algorithm
string quicksort_kernel() {
 return string("\n") +
     string("__kernel void quicksort(__global int* data, int start, int end) {\n") +
     string(" int gid = get_global_id(0);\n") +
     string(" if (gid >= start && gid <= end) {\n") +
     string("  int pivot = data[end];\n") +
     string("  int i = start - 1;\n") +
     string("  for (int j = start; j <= end; j++) {\n") +
     string("   if (data[j] < pivot) {\n") +
     string("    i++;\n") +
     string("    int temp = data[j];\n") +
     string("    data[j] = data[i];\n") +
     string("    data[i] = temp;\n") +
     string("   }\n") +
     string("  }\n") +
     string("  data[i + 1] = data[end];\n") +
     string(" }\n") +
     string("}\n");
}

// Logic for the OpenCL quick sort implementation
void perform_ocl_qs(vector<int>& local_data, int start, int end) {
 // Initialising OCL for each of the process
 cl_int err;
 cl_platform_id platform;
 cl_device_id device;
 cl_context context;
 cl_command_queue queue;
 cl_program program;
 cl_kernel kernel;

 // Creating the context
 err = clGetPlatformIDs(1, &platform, nullptr);
 err |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, nullptr);
 context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
 if (err != CL_SUCCESS) {
  cerr << "Failed to create OpenCL context" << endl;
  exit(EXIT_FAILURE);
 }

 // Creating the command queue
 queue = clCreateCommandQueue(context, device, 0, &err);
 if (err != CL_SUCCESS) {
  cerr << "Failed to create command queue" << endl;
  exit(EXIT_FAILURE);
 }

 // Loading the kernel source code
 string kernelSource = quicksort_kernel();
 const char *KernelSource = kernelSource.c_str();

 // Creating and building the program
 program = clCreateProgramWithSource(context, 1, &KernelSource, nullptr, &err);
  
 if (err != CL_SUCCESS) {
  cerr << "Failed to create program" << endl;
  exit(EXIT_FAILURE);
 }

 err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
 if (err != CL_SUCCESS) {
  cerr << "Failed to build program" << endl;

  // Get build log
  size_t logSize;
  clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
  char *log = new char[logSize];
  clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr);
  cerr << "Build log:\n" << log << endl;
  delete[] log;

  exit(EXIT_FAILURE);
 }

 // Create kernel
 kernel = clCreateKernel(program, "quicksort", &err);
 if (err != CL_SUCCESS) {
  cerr << "Failed to create kernel" << endl;
  exit(EXIT_FAILURE);
 }

 // Create buffer
 cl_mem bufData = clCreateBuffer(context, CL_MEM_READ_WRITE, local_data.size() * sizeof(int), nullptr, &err);
 if (err != CL_SUCCESS) {
  cerr << "Failed to create buffer" << endl;
  exit(EXIT_FAILURE);
 }

 // Write data to buffer
 err = clEnqueueWriteBuffer(queue, bufData, CL_TRUE, 0, local_data.size() * sizeof(int), local_data.data(), 0, nullptr, nullptr);
 if (err != CL_SUCCESS) {
  cerr << "Failed to write buffer" << endl;
  exit(EXIT_FAILURE);
 }

 // Set kernel arguments
 err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufData);
 err |= clSetKernelArg(kernel, 1, sizeof(int), &start);
 err |= clSetKernelArg(kernel, 2, sizeof(int), &end);
 if (err != CL_SUCCESS) {
  cerr << "Failed to set kernel arguments" << endl;
  exit(EXIT_FAILURE);
 }

 // Enqueue kernel (assuming local_data size fits work-group size)
 size_t globalSize = local_data.size();
 err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);
 if (err != CL_SUCCESS) {
  cerr << "Failed to enqueue kernel" << endl;
  exit(EXIT_FAILURE);
 }

 // Read data back
 err = clEnqueueReadBuffer(queue, bufData, CL_TRUE, 0, local_data.size() * sizeof(int), local_data.data(), 0, nullptr, nullptr);
 if (err != CL_SUCCESS) {
  cerr << "Failed to read buffer" << endl;
  exit(EXIT_FAILURE);
 }

 // Clearing up the OCL objects
 clReleaseMemObject(bufData);
 clReleaseProgram(program);
 clReleaseKernel(kernel);
 clReleaseCommandQueue(queue);
 clReleaseContext(context);
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


int main(int argc, char **argv) {
 srand(time(0));
  
 int num_procs, rank;
 MPI_Init(&argc, &argv);
 MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);

 // Filling an array with random values
 vector<int> data;
 if (rank == 0) 
  create_array(data, array_size);
	
 // Broadcast the size of array to all the processes
 int vector_size = data.size();
 MPI_Bcast(&vector_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

 // Scatter the data to all processes
 vector<int> local_data(vector_size / num_procs);
 MPI_Scatter(data.data(), vector_size / num_procs, MPI_INT, local_data.data(), vector_size / num_procs, MPI_INT, 0, MPI_COMM_WORLD);

  auto start = high_resolution_clock::now();
  // Implementing the OCL quick sort
  perform_ocl_qs(local_data, 0, local_data.size() - 1);
auto end = high_resolution_clock::now();
 // Gather the sorted data to master process
 MPI_Gather(local_data.data(), vector_size / num_procs, MPI_INT, data.data(), vector_size / num_procs, MPI_INT, 0, MPI_COMM_WORLD);

 if (rank == 0) {
  final_merge(data, 0, array_size-1);
  auto duration = duration_cast<microseconds> (end - start);

  // Printing the execution time
  cout << "Execution Time: " << duration.count() << " microseconds" << endl;
 }

	// if(rank==0)
	// for(int i = 0; i < array_size; i++) cout << data[i] << " ";
	// cout<<endl;
 MPI_Finalize();
	

 return 0;
}