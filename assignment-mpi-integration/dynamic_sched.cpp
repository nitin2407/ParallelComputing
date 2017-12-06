#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <chrono>

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

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int rank, size;
  int gran;
  double total_sum=0.0;

  std::chrono::time_point<std::chrono::system_clock> start_time; 
  std::chrono::time_point<std::chrono::system_clock> end;

  MPI_Init (&argc, &argv);

  int function = atoi(argv[1]);
  float a = atof(argv[2]); 
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  long intensity = atol(argv[5]);
  float temp = (b-a)/n;


  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  gran = 0.1*n;
  
  if(rank==0){
    //Master
    start_time=std::chrono::system_clock::now();
    MPI_Status stat_master;
    double sum_temp=0.0;
    int current=0;
    for(int i=1;i<size;i++){
      MPI_Send(&current, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      if(current+gran<n){
        current+=gran;
      }else{
        current=n;
      }
    }
    while(current<n){
      MPI_Recv(&sum_temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat_master);
		  total_sum += sum_temp;
		  MPI_Send(&current, 1, MPI_INT, stat_master.MPI_SOURCE, 1, MPI_COMM_WORLD);
		  current += gran;
    }
    for(int i=1;i<size;i++){
      MPI_Recv(&sum_temp,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&stat_master);
      total_sum+=sum_temp;
      MPI_Send(0,0,MPI_INT,i,2,MPI_COMM_WORLD);
    }
    end = std::chrono::system_clock::now();
  }else{
    //Worker
    double sum=0.0;
    int start,stop;
    MPI_Status stat_worker;
    while(true){
      MPI_Recv(&start,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD, &stat_worker);
      if(stat_worker.MPI_TAG==2){
        break;
      }
      if(start+gran>n){
        stop=n;
      }else{
        stop=start+gran;
      }
      sum=0.0;
      //cout<<stop<<endl;
      for(int i=start;i<stop;i++){
        if(function==1){
          sum+=f1(a+((i+0.5)*temp),intensity)*temp;
        }
        else if(function==2)
        {
          sum+=f2(a+((i+0.5)*temp),intensity)*temp;  
        }
        else if(function==3)
        {
          sum+=f3(a+((i+0.5)*temp),intensity)*temp;  
        }
        else if(function==4)
        {
          sum+=f4(a+((i+0.5)*temp),intensity)*temp;  
        }
      }
      MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
  }
  MPI_Finalize();
  if(rank==0){  
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    cout<<total_sum<<endl;
  }
  return 0;
}
