#include <iostream>
#include <chrono>

#define M 512

__device__ float polynomial (float x, float* poly, int degree) {
  float out = 0.;
  float xtothepowerof = 1.;
  for (int i=0; i<=degree; ++i) {
    out += xtothepowerof*poly[i];
    xtothepowerof *= x;
  }
  return out;
}

__global__ void polynomial_expansion (float* poly, int degree,
			   int n, float* array) {

  int index = threadIdx.x + blockIdx.x*blockDim.x;
  
    if(index<n)
    {
      array[i] = polynomial (array[i], poly, degree);  
    }

}


int main (int argc, char* argv[]) {
  //TODO: add usage
  
  if (argc < 3) {
     std::cerr<<"usage: "<<argv[0]<<" n degree"<<std::endl;
     return -1;
  }

  int n = atoi(argv[1]); //TODO: atoi is an unsafe function
  int degree = atoi(argv[2]);
  int nbiter = 1;

  float* array,*poly;
  int size_array = n*sizeof(float);
  int size_poly = (degree+1)*sizeof(float);

  float* d_array,*d_poly;

  array = (float *)malloc(size_array);
  poly = (float *)malloc(size_poly);

  cudaMalloc((void **)&d_array,size_array);
  cudaMalloc((void **)&d_poly,size_poly;


  for (int i=0; i<n; ++i)
    array[i] = 1.;

  for (int i=0; i<degree+1; ++i)
    poly[i] = 1.;

  cudaMemcpy(d_array,array,size_array,cudaMemcpyHostToDevice);
  cudaMemcpy(d_poly,poly,size_poly,cudaMemcpyHostToDevice);
  
  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();
  
  for (int iter = 0; iter<nbiter; ++iter)
    polynomial_expansion<<<(n+M-1)/M,M>>>(poly, degree, n, array);

  cudaMemcpy(array,d_array,size_array,cudaMemcpyDeviceToHost);

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin)/nbiter;

  std::cerr<<array[0]<<std::endl;
  std::cout<<n<<" "<<degree<<" "<<totaltime.count()<<std::endl;

  free(array);
  free(poly);

  cudaFree(d_array);
  cudaFree(d_poly);

  return 0;
}
