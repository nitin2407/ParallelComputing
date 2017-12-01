#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <chrono>

using namespace std;

int n,power;
double** H;

void rand_fill_values()
{
    for(int i = 0;i<n; i++)
    {
        for(int j = 0;j<n;j++)
        {
            H[i][j] = (rand()%MAX_NUMBER + 1)*1.0;
        }
    }
    return;
}

double calculate_value(int i, int j)
{
    double result = 0.0;
    if(i == 0 || i == n-1 || j == 0 || j == n-1)
    {
        if((i == 0 && j == 0))
        {
            result = 6*H[i][j] + H[i][j+1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(i == 0 && j == n-1)
        {
            result = 6*H[i][j] + H[i][j-1] + H[i+1][j] + H[i+1][j-1];
        }
        else if(i == n-1 && j == 0)
        {
            result = 6*H[i][j] + H[i][j+1] + H[i-1][j] + H[i-1][j+1];
        }
        else if(i == n-1 && j == n-1)
        {
            result = 6*H[i][j] + H[i-1][j] + H[i][j-1] + H[i-1][j-1];
        }
        else if(i == 0)
        {
            result = H[i][j-1] + 4*H[i][j] + H[i][j+1] + H[i+1][j-1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(i == n-1)
        {
            result = H[i-1][j-1] + H[i-1][j] + H[i-1][j+1] + H[i][j-1] + 4*H[i][j] + H[i][j+1];
        }
        else if(j == 0)
        {
            result = H[i-1][j] + H[i-1][j+1] + 4*H[i][j] + H[i][j+1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(j == n-1)
        {
            result = H[i-1][j-1] + H[i-1][j] + H[i][j-1] + 4*H[i][j] + H[i+1][j-1] + H[i+1][j];
        }
    }
    else
    {
        result = H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j-1]+H[i][j]+H[i][j+1]+H[i+1][j-1]+H[i+1][j]+H[i+1][j+1];
    }
    return result/9.0;
}

void cal_heat_equation(int rank){
  int k;
  double *temp=new double[n*(n/P)];
  int start=(rank)*n*(n/P);
  int stop = (rank+1)*n*(n/P);
  int cur_power=0;

  while(power--){

    k=0;
    int j,l;
    for(int i=start;i<stop;i++){
      j=i%n;
      l=i/n;
      temp[k]=cal_heat_equation(l,j);
      k++;
    }

    if(rank!=0){
      MPI_Send(temp,n*(n/P),MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    }
    else{
      for(int i=start;i<stop;i++){
        H[i]=temp[i];
      }

      int next_start,next_stop;
      for(int i=0;i<P;i++){
        next_start = i*n*(n/P);
        next_stop = (i+1)*n*(n/P);
        MPI_Recv(temp,n*(n/P),MPI_DOUBLE,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        k=0;
        for(int j=next_start;j<next<stop;j++){
          H[j]=temp[k];
          k++;
        }
      }
    }
    MPI_Bcast(H,n*n,MPI_DOUBLE,root,MPI_COMM_WORLD);
    cur_power++;
  }
  delete[] temp;
  return;
}

int main (int argc, char* argv[]) {

    srand(time(NULL));
    if (argc < 3) {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <power>"<<std::endl;
        return -1;
    }

  int rank, size;
  int gran;
  double total_sum=0.0;

  std::chrono::time_point<std::chrono::system_clock> start_time; 
  std::chrono::time_point<std::chrono::system_clock> end;

  MPI_Init (&argc, &argv);

  int n = atoi(argv[1]);
  int power = atoi(argv[2]); 

  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  //gran = 0.1*n;
  
    //start_time=std::chrono::system_clock::now();
    //end = std::chrono::system_clock::now();
  cal_heat_equation(rank);
  
  MPI_Finalize();
  //std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  if(rank==0){  
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    //cout<<total_sum<<endl;
  }
  return 0;
}
