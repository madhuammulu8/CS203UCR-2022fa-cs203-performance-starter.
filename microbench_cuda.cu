#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include "microbench_cuda.h"
#define BLOCK_SIZE 1024


extern "C" 
uint64_t *baseline_double_cuda(uint64_t * _array, unsigned long int size)
{
         double *d_array, *d_output;
         cudaMalloc((void **) &d_array, sizeof(double)*size);
         cudaMalloc((void **) &d_output, sizeof(double)*size);
         cudaMemcpy(d_array, _array, sizeof(double)*size, cudaMemcpyHostToDevice);

         unsigned int grid_cols = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

         // Launch kernel 
         baseline_double_cuda_init<<<grid_cols, BLOCK_SIZE>>>(d_array, size);
         for(int j=0;j<3;j++)
         {
             baseline_double_cuda_kernel<<<grid_cols, BLOCK_SIZE>>>(d_array, d_output, size, j);
             cudaThreadSynchronize();
             cudaMemcpy(d_array, d_output, sizeof(double)*size, cudaMemcpyDeviceToDevice);
         }
         // Transefr results from device to host 
         cudaMemcpy(_array, d_output, sizeof(double)*size, cudaMemcpyDeviceToHost);
         cudaFree(d_output);
         cudaFree(d_array);
	 return _array;
}
__global__ void baseline_double_cuda_kernel(double *input, double *output, int size, int j) 
{
    unsigned int pos = blockIdx.x * blockDim.x + threadIdx.x;

    if (pos < size)
    {
        output[pos] = input[pos]+pos/(j+1)+input[pos-1];
    }
}
__global__ void baseline_double_cuda_init(double *array, int size) 
{
    unsigned int pos = blockIdx.x * blockDim.x + threadIdx.x;

    if (pos < size) 
    {
        array[pos] = 0;
    }
}
