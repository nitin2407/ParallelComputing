#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

using namespace std;

int functionid, a, b ;
long n, intensity;
float temp;
int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  
  int rank,start,stop,P;
  float sum=0;
  MPI_Init (&argc, &argv);

  functionid = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atol(argv[4]);
  intensity = atoi(argv[5]);
  float intgr=0;
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &P);
  
  MPI_Barrier(MPI_COMM_WORLD); 
  std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();

  start = (n/P)*rank;
  if(rank==(P-1)){
    stop=n;
  }else{
    stop=(n/P)*(rank+1);
  }
  temp = (b-a)/(float)n;
  MPI_Status stat;
  for(int i=start;i<stop;i++){
    if(functionid==1){
      intgr = f1(a+((float)(i+0.5)*temp),intensity)*temp;
    }
    else if(functionid==2)
    {
      intgr = f2(a+((float)(i+0.5)*temp),intensity)*temp;  
    }
    else if(functionid==3)
    {
      intgr = f3(a+((float)(i+0.5)*temp),intensity)*temp;  
    }
    else if(functionid==4)
    {
      intgr = f4(a+((float)(i+0.5)*temp),intensity)*temp;  
    }
    sum = sum + intgr;
  }
  if(rank!=0){
    MPI_Send(&sum,1,MPI_FLOAT,0,1,MPI_COMM_WORLD);
  }else{
    float insum=0;
    for(int i=1;i<P;i++){
      MPI_Recv(&insum,1,MPI_FLOAT,i,1,MPI_COMM_WORLD,&stat);
      sum+=insum;
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_time;

    std::cerr<<elapsed_seconds.count()<<std::endl;
    cout<<sum<<endl;
  }

  MPI_Finalize();
  return 0;
}