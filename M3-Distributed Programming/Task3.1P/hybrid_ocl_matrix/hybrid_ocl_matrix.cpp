/* This code is made by Anneshu Nag, Student ID: 2210994760 */
/*  This is MPI + OpenCL based hybrid Matrix Multiplication */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <chrono>

#define CL_TARGET_OPENCL_VERSION 300
#include<CL/cl.h>

using namespace std::chrono;
#define matrixSize 1000 // Defining the size of the Matrix

int matrix1[matrixSize][matrixSize];
int matrix2[matrixSize][matrixSize];
int res[matrixSize][matrixSize];
cl_mem bufA, bufB, bufC;

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel; // Kernel function
cl_command_queue  queue;
cl_event event = NULL;

int err;
const int max = matrixSize;
const int TS = 4;
const size_t local[2] = { (size_t)TS, (size_t)TS };
const size_t global[2] = { (size_t) max, (size_t)max }; 

cl_device_id create_device();
void setup_openCL_device_context_queue_kernel(char* filename, char* kernelname);
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

// Declaring the functions to be used
void master(int proc, int start, int end);
void headOrnode(int proc, int start, int end);
void create_matrix(int matrix[matrixSize][matrixSize]);
void output(int matrix[matrixSize][matrixSize]);
void del(int matrix[matrixSize][matrixSize]);

struct req_com_vars
{
    int proc_rank, proc, start, end, num_threads;
};
req_com_vars values;

// Creating the matrices for the multiplication program
void create_matrix(int matrix[matrixSize][matrixSize])
{
  for (int i=0; i<matrixSize; i++)
    for (int j=0; j<matrixSize; j++)
      matrix[i][j] = rand() % 10;
}

// Creating the Platform and Device 
cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   // Find the platform avaiable
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("No platform available.");
      exit(1);
   } 
   
   // Find the device
   // Use GPU if available
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // If GPU not available use CPU
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("No device available.");
      exit(1);   
   }
   return dev;
}

// Main function
int main(int argc, char *argv[])
{
    // Initialising MPI  
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &values.proc_rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &values.proc);    

    values.num_threads = 1;  // Defining the number of threads to be generated (1 per process/core)
    values.start = values.proc_rank * matrixSize / values.proc;
    values.end = ((values.proc_rank + 1) * matrixSize / values.proc);
    
      auto start = high_resolution_clock::now(); // Start time measurement
    if (values.proc_rank == 0){
    }
    if (values.proc_rank == 0)
    {
        master(values.proc, values.start, values.end);
    }
    else
    {
        headOrnode(values.proc, values.start, values.end);
    }

    if (values.proc_rank == 0){
        auto stop = high_resolution_clock::now(); // Stop time measurement
        auto duration = duration_cast<microseconds>(stop - start); // Calculate duration
        std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
        free_memory();
    }
    MPI_Finalize();
    return 0;
}

void master(int proc, int start, int end){
    create_matrix(matrix1); create_matrix(matrix2);

    MPI_Bcast(matrix2, matrixSize * matrixSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&matrix1[0][0], matrixSize * matrixSize / proc, MPI_INT, MPI_IN_PLACE, matrixSize * matrixSize / proc, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = start; i < end; i++)
        for (int j = 0; j < matrixSize; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < matrixSize; k++)
                res[i][j] += matrix1[i][k] * matrix2[k][j];
        }

    MPI_Gather(MPI_IN_PLACE, matrixSize*matrixSize/proc, MPI_INT, &res[0][0], matrixSize*matrixSize/proc, MPI_INT, 0, MPI_COMM_WORLD);

    // Setting up the Kernel
    setup_openCL_device_context_queue_kernel( (char*) "./matrix_multiplication_kernel.cl" , (char*) "multiply_matrices");
    setup_kernel_memory();
    copy_kernel_args();
    // Preparing the kernel for execution 
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    clWaitForEvents(1, &event);
   // Copying data from the device back to resultant matrix
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, matrixSize * matrixSize *sizeof(int), &res[0][0], 0, NULL, NULL);
}

void headOrnode(int proc, int start, int end){
    MPI_Bcast(matrix2, matrixSize * matrixSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&matrix1[0][0], matrixSize * matrixSize / proc, MPI_INT, &matrix1[start], matrixSize * matrixSize / proc, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = start; i < end; i++)
        for (int j = 0; j < matrixSize; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < matrixSize; k++)
                res[i][j] += matrix1[i][k] * matrix2[k][j];
        }

     MPI_Gather(&res[start], matrixSize*matrixSize/proc, MPI_INT, &res, matrixSize*matrixSize/proc, MPI_INT, 0, MPI_COMM_WORLD);

}

void free_memory() {
   // Freeing the buffers
   clReleaseMemObject(bufA);
   clReleaseMemObject(bufB);
   clReleaseMemObject(bufC);

   // Freeing the OpenCL Objects
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
}
void copy_kernel_args() {
    clSetKernelArg(kernel, 0, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 1, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufA);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufB);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufC);
    if(err < 0) {
      perror("Failed to create a kernel argument");
      printf("error = %d", err);
      exit(1);
   }
}
void setup_kernel_memory() {
     bufA = clCreateBuffer(context, CL_MEM_READ_ONLY,  matrixSize*matrixSize*sizeof(int), NULL, NULL);
     bufB = clCreateBuffer(context, CL_MEM_READ_ONLY,  matrixSize*matrixSize*sizeof(int), NULL, NULL);
     bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, matrixSize*matrixSize*sizeof(int), NULL, NULL);

    // Copy matrices to the GPU/CPU
    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, matrixSize*matrixSize*sizeof(int), &matrix1[0][0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, matrixSize*matrixSize*sizeof(int), &matrix2[0][0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, matrixSize*matrixSize*sizeof(int), &res[0][0], 0, NULL, NULL);
}

void setup_openCL_device_context_queue_kernel(char* filename, char* kernelname) {
    device_id = create_device();
    cl_int err;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
   if(err < 0) {
      perror("Context creation failed");
      exit(1);   
    }

    program = build_program(context, device_id, filename );
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if(err < 0) {
      perror("Failed in creating command queue");
      exit(1);   
    };

    kernel = clCreateKernel(program, kernelname, &err);
    if(err < 0) {
      perror("Kernel creation failed");
      printf("error =%d", err);
      exit(1);
    };

}
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;

   // Read the kernel file
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Kernel openeing failed");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   // Creating a program using the kernel 
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Program creation failed");
      exit(1);
   }
   free(program_buffer);
  
   // Building the program
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }
   return program;
}

