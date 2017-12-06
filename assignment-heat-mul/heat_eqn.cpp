#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <math.h>
#include <chrono>

using namespace std;

#define MAX_NUMBER 1000
int n,power;
double *H;
const int root=0;


void rand_fill_values()
{
	for(int i = 0;i<n; i++)
	{
		for(int j = 0;j<n;j++)
		{
			H[i*n + j] = (rand()%MAX_NUMBER + 1)*1.0;
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
			result = 6*H[i*n + j] + H[i*n + j+1] + H[(i+1)*n + j] + H[(i+1)*n + j+1];
		}
		else if(i == 0 && j == n-1)
		{
			result = 6*H[i*n + j] + H[i*n + j-1] + H[(i+1)*n + j] + H[(i+1)*n + j - 1];
		}
		else if(i == n-1 && j == 0)
		{
			result = 6*H[i*n + j] + H[i*n + j+1] + H[(i-1)*n +j] + H[(i-1)*n + j+1];  
		}
		else if(i == n-1 && j == n-1)
		{
			result = 6*H[(i)*n + j] + H[(i-1)*n + j] + H[i*n + j-1] + H[(i-1)*n + j-1];
		}
		else if(i == 0)
		{
			result = H[i*n + j-1] + 4*H[i*n + j] + H[i*n + j+1] + H[(i+1)*n + j-1] + H[(i+1)*n + j] + H[(i+1)*n + j+1];
 		}
 		else if(i == n-1)
		{
			result = H[(i-1)*n + j-1] + H[(i-1)*n + j] + H[(i-1)*n + j+1] + H[i*n + j-1] + 4*H[i*n + j] + H[i*n + j+1];
 		}
 		else if(j == 0)
		{
			result = H[(i-1)*n + j] + H[(i-1)*n + j+1] + 4*H[i*n + j] + H[i*n + j+1] + H[(i+1)*n + j] + H[(i+1)*n + j+1];
 		}
 		else if(j == n-1)
		{
			result = H[(i-1)*n + j-1] + H[(i-1)*n + j] + H[i*n + j-1] + 4*H[i*n + j] + H[(i+1)*n + j-1] + H[(i+1)*n + j];
 		}
	}
	else
	{
		result = H[(i-1)*n + j-1]+H[(i-1)*n + j]+H[(i-1)*n + j+1]+H[i*n + j-1]+H[i*n + j]+H[i*n + j+1]+H[(i+1)*n + j-1]+H[(i+1)*n + j]+H[(i+1)*n + j+1];
	}
  return result/9.0;
}

void cal_heat_equation(int rank,int size){
  int k;
  double *temp = (double*)malloc(n*(n/size)*sizeof(double));
  //new double[n*(n/size)];
  int start=(rank)*n*(n/size);
  int stop = (rank+1)*n*(n/size);
  int cur_power=0;
  //cout<<start<<endl;
  //cout<<stop<<endl;
  while(power--){
    //cout<<cur_power<<endl;
    k=0;
    int j,l;
    for(int i=start;i<stop;i++){
      //cout<<"i"<<i<<endl;
      j=i%n;
      l=i/n;
      temp[k]=calculate_value(l,j);
      k++;
    }
    //cout<<"k"<<k<<endl;
    //cout<<"starting"<<endl;
    if(rank!=0){
      //cout<<"sending"<<endl;
      MPI_Send(temp,n*(n/size),MPI_DOUBLE,0,0,MPI_COMM_WORLD);
      //cout<<"data sent"<<endl;
    }
    else
    {
      //cout<<"receiving"<<endl;
      for(int i=start;i<stop;i++)
      {
        H[i]=temp[i];
      }

      int next_start,next_stop;
      for(int i=1;i<size;i++){
        next_start = i*n*(n/size);
        next_stop = (i+1)*n*(n/size);
        MPI_Recv(temp,n*(n/size),MPI_DOUBLE,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        k=0;
        for(int j=next_start;j<next_stop;j++)
        {
          H[j]=temp[k];
          k++;
        }
      }
    }
    MPI_Bcast(H,n*n,MPI_DOUBLE,root,MPI_COMM_WORLD);
    cur_power++;
  }
  free(temp);
  return;
}

int main (int argc, char* argv[]) {

  srand(time(NULL));
  if (argc < 3) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <power>"<<std::endl;
    return -1;
  }

  n = atoi(argv[1]);
  power = atoi(argv[2]); 
  H = (double*)malloc(n*n*sizeof(double));
  //H = new double[n*n];

  rand_fill_values();
  
  int rank,size;
  
  MPI_Init (&argc, &argv);

  MPI_Barrier(MPI_COMM_WORLD);
  std::chrono::time_point<std::chrono::system_clock> start_time=std::chrono::system_clock::now();

  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  //cout<<"rank "<<rank<<endl;
  //cout<<"size "<<size<<endl;
  cal_heat_equation(rank,size);

  MPI_Finalize();
  //std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  if(rank==0){   
    //MPI_Barrier(MPI_COMM_WORLD);
    std::chrono::time_point<std::chrono::system_clock> end=std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
  }
  //delete[] H;
  free(H);
  return 0;
}
