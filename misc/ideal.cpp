#include <math.h>
#include <stddef.h>
#include <thread>
#include <vector>
#include <iostream>

struct dim3 {
  unsigned x, y, z;
  dim3(unsigned x, unsigned y = 1, unsigned z = 1) : x(x), y(y), z(z) {}
};

void vecAdd(double *a, double *b, double *c, int n, dim3 gridDim, dim3 blockDim, dim3 blockIdx)
{
  dim3 threadIdx(blockDim.x, blockDim.y, blockDim.z);
  for(threadIdx.x=0; threadIdx.x < blockDim.x; threadIdx.x++){
//  for(auto threadIdx.x : blockDim.x){
    for(threadIdx.y=0; threadIdx.y < blockDim.y; threadIdx.y++){
//    for(auto threadIdx.y : blockDim.y){
      for(threadIdx.z=0; threadIdx.z < blockDim.z; threadIdx.z++){
//      for(auto threadIdx.z : blockDim.z){
        int id = blockIdx.x*blockDim.x+threadIdx.x;
        if(id<n){
          c[id] = a[id] + b[id];
        }
      }
    }
  }
}

int main()
{
  int n = 1000;
  double *h_a, *h_b, *h_c;
  double *d_a, *d_b, *d_c;
  size_t bytes = n*sizeof(double);
  h_a = (double*)malloc(bytes);
  h_b = (double*)malloc(bytes);
  h_c = (double*)malloc(bytes);
  int i;
  for( i = 0; i < n; i++ ) {
    h_a[i] = sin(i)*sin(i);
    h_b[i] = cos(i)*cos(i);
  }
  int blockSize, gridSize;
  blockSize = 1024;
  gridSize = (int)ceil((float)n/blockSize);
  unsigned int AVAILABLE_THREADS = 8; //std::thread::hardware_concurrency();
  std::cout << AVAILABLE_THREADS << "\n\n";
  auto nSteps = (gridSize+AVAILABLE_THREADS-1)/AVAILABLE_THREADS;
  std::vector<std::thread> t;
  for(int tid=0; tid < AVAILABLE_THREADS; ++tid){
      for(auto k=0; k < nSteps; ++k){
    	  auto blockIdx = tid + AVAILABLE_THREADS*k;
    	  if(blockIdx < blockSize)
    		  t.push_back(std::thread(vecAdd, h_a, h_b, h_c, n, gridSize, blockSize, blockIdx));
      }
  }


  for(auto& thread : t)
	  thread.join();

  double sum = 0;
  for(i=0; i<n; i++)
      sum += h_c[i];
  std::cout << "final result: " << sum/n << "\n";

  free(h_a);
  free(h_b);
  free(h_c);
  return 0;
}
