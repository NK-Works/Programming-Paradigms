/* This code is made by Anneshu Nag, Student ID: 2210994760 */ 
#include <iostream> 
#include <vector> 
#include <chrono> 
#include <CL/cl.h> 
using namespace std; 
 
// Function to perform matrix multiplication using OpenCL 
void openCLMatrixMultiplication(const vector<vector<int>> &A, const vector<vector<int>> &B, 
vector<vector<int>> &C, int n) 
{ 
    // OpenCL setup 
    cl_platform_id platform; 
    clGetPlatformIDs(1, &platform, nullptr); 
 
    cl_device_id device; 
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr); 
 
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr); 
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, nullptr); 
 
    // Create OpenCL buffers 
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) 
* n * n, (void*)A.data(), nullptr); 
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) 
* n * n, (void*)B.data(), nullptr); 
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * n * n, nullptr, 
nullptr); 
 
    // Load and compile the OpenCL program 
    const char *kernelSource = R"( 
        _kernel void matrixMultiply(_global int* A, __global int* B, __global int* C, const int n) 
{ 
            int i = get_global_id(0); 
            int j = get_global_id(1); 
 
            int sum = 0; 
            for (int k = 0; k < n; ++k) { 
                sum += A[i * n + k] * B[k * n + j]; 
            } 
 
            C[i * n + j] = sum; 
        } 
    )"; 
 
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, nullptr); 
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr); 
 
    // Execute the OpenCL kernel 
    cl_kernel kernel = clCreateKernel(program, "matrixMultiply", nullptr); 
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA); 
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB); 
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC); 
    clSetKernelArg(kernel, 3, sizeof(int), &n); 
 
    size_t globalSize[2] = { static_cast<size_t>(n), static_cast<size_t>(n) }; 
    clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, nullptr, 0, nullptr, nullptr); 
    clFinish(queue); 
 
    // Read the result back from the OpenCL device 
    clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(int) * n * n, C.data(), 0, nullptr, 
nullptr); 
 
    // Release OpenCL resources 
    clReleaseMemObject(bufferA); 
    clReleaseMemObject(bufferB); 
    clReleaseMemObject(bufferC); 
    clReleaseKernel(kernel); 
    clReleaseProgram(program); 
    clReleaseCommandQueue(queue); 
    clReleaseContext(context); 
} 
 
int main() 
{ 
    const int n = 1000; 
    vector<vector<int>> A(n, vector<int>(n)); 
    vector<vector<int>> B(n, vector<int>(n)); 
    vector<vector<int>> C(n, vector<int>(n)); 
 
    // Initialize input matrices A and B 
    srand(time(nullptr)); 
    for (int i = 0; i < n; ++i) 
    { 
        for (int j = 0; j < n; ++j) 
        { 
            A[i][j] = rand() % 10; 
            B[i][j] = rand() % 10; 
        } 
    } 
    auto start = chrono::steady_clock::now(); 
    openCLMatrixMultiplication(A, B, C, n); 
    auto end = chrono::steady_clock::now(); 
    cout << "OpenCL time: " << chrono::duration_cast<chrono::duration<double>>(end - 
start).count() << " seconds" << endl; 
    return 0; 
} 
