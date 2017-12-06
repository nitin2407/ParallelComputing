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
    MPI_Request req_f,req_s;
    double sum_temp=0.0;
    int current_pending=0;
    int current=0;
    int last_value=-2;
    for(int i=1;i<size;i++)
    {
      for(int j=0;j<3;j++)
      {
        //cout<<"sending initial"<<endl;
        if(current<n)
        {
          MPI_Isend(&current, 1, MPI_INT, i, 1, MPI_COMM_WORLD,&req_f);
          current_pending++;
          if(current+gran<n){
            current+=gran;
          }else{
            current=n;
          }
        }
      }
      if(current>=n)
        break;
    }
    while(current<n){
      //cout<<"sending next data"<<endl;
      MPI_Irecv(&sum_temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &req_s);
      MPI_Wait(&req_s,&stat_master);
      current_pending--;
      total_sum += sum_temp;
      //cout<<total_sum<<endl;
		  MPI_Isend(&current, 1, MPI_INT, stat_master.MPI_SOURCE, 0, MPI_COMM_WORLD,&req_f);
      current_pending++;
		  if(current+gran<n){
          current+=gran;
      }else{
        current=n;
      }
    }
    while(current_pending--){
      //cout<<"receiving final values"<<endl;
      MPI_Irecv(&sum_temp,1,MPI_DOUBLE,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&req_s);
      MPI_Wait(&req_s,&stat_master);
      total_sum+=sum_temp;
    }
    for(int i=1;i<size;i++){
      MPI_Isend(&last_value,1,MPI_INT,i,0,MPI_COMM_WORLD,&req_f);
    }
    MPI_Wait(&req_f,&stat_master);
    end = std::chrono::system_clock::now();
  }else{
    //Worker
    int* receive_data = new int[n];
    memset(receive_data,-1,n*sizeof(int));
    double sum=0.0;
    int start,stop;
    MPI_Status stat_worker;
    MPI_Request req_recv[n];
    MPI_Request req_s;
    int cur_idx=0,idx=0;
    for(int i=0;i<3;i++){
      MPI_Irecv(&receive_data[cur_idx],1,MPI_DOUBLE, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req_recv[cur_idx]);
		  cur_idx++;
    }
    MPI_Wait(&req_recv[0],MPI_STATUS_IGNORE);
    while(receive_data[idx]!=-2){
      sum=0;
      start=receive_data[idx];
      if(start+gran>=n){
        stop=n;
      }else{
        stop=start+gran;
      }
      for(int i=start;i<stop;i++)
      {
        if(function==1)
        {
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
      MPI_Isend(&sum,1,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&req_s);
      idx++;
      MPI_Irecv(&receive_data[cur_idx],1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&req_recv[cur_idx]);
      cur_idx++;
      if(receive_data[idx]==-1){
        MPI_Wait(&req_recv[idx],MPI_STATUS_IGNORE);
      }
    }
    //cout<<"done "<<rank<<endl;
    delete[] receive_data; 
  }
  MPI_Finalize();
  //std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  if(rank==0){  
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    cout<<total_sum<<endl;
  }
  return 0;
}
