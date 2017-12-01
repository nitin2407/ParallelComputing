#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <chrono>

using namespace std;

#define MAX_NUMBER 10
int n,power,s;
long long int *x;
int **A;
const int root=0;

void rand_fill_values()
{
    for(int i = 0;i<n; i++)
    {
        x[i] = rand()%MAX_NUMBER + 1;
        /*for(int j = 0;j<n;j++)
        {
            A[i][j] = rand()%MAX_NUMBER + 1;
        }*/
    }
    return;
}

void Dense_Block(int rank)
{
	int k,l;
	long long int *temp = new long long int[n/s + 1];
	long long int *temp2 = new long long int[n/s +1];
	int starty = (rank%s)*n/s;
	int endy = (rank%s+1)*n/s;

	int startx = (rank/s)*n/s;
	int endx = (rank/s+1)*n/s;

	long long int *y = new long long int[n/s];

	A = new int*[endx-startx+1];	
	for(int i = 0; i < (endx-startx); i++)
	{
		A[i] = new int[endy-starty+1];
		for(int j = 0;j<(endy-starty);j++)
		{
			A[i][j] = rand()%MAX_NUMBER + 1;
		}
	}

	while(power--)
	{
		k = 0;
		for(int i = 0; i<(endx-startx);i++) 
		{
			y[k] = 0;
			l = 0;
			for(int j = starty;j<endy;j++)
			{
				y[k] += A[i][l]*x[j];
				l++;
			}
			k++;
		}

		int starti, endi;
		if(rank%s != 0)
		{
			MPI_Send(y, (endx-startx), MPI_LONG_LONG_INT, (rank-rank%s), 0, MPI_COMM_WORLD);
		}
		else
		{
		    int starti, endi;

			for(int i = 1;i<s;i++)
			{
				starti = (i)*n/s;
				endi = (i+1)*n/s;

				MPI_Recv(temp, (endi-starti), MPI_LONG_LONG_INT, rank+i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				for(int j = 0;j<endx-startx;j++)
				{
					y[j] += temp[j];
				}
			}

			if(rank == 0)
			{
				for(int i = startx;i<endx;i++)
				{
					x[i] = y[i];
				}
				for(int i = 1;i<s;i++)
				{
					MPI_Recv(temp2, n/s, MPI_LONG_LONG_INT, i*s, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					starti = i*n/s;
					endi = (i+1)*n/s;
					k = 0;
					for(int j = starti;j<endi;j++)
					{
						x[j] = temp2[k];
						k++;
					}
				}
			}
			else
			{
				MPI_Send(y, (endx-startx), MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
			}
		}

		MPI_Bcast(x, n, MPI_LONG_LONG_INT, root, MPI_COMM_WORLD);

	}

	delete[] y;
	delete[] temp;
	delete[] temp2;
	return; 
}

int main (int argc, char* argv[]) {

    srand(time(NULL));

    if(argc != 3)
    {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <power>"<<std::endl;
        return -1;
    }

	n = atoi(argv[1]);
    power = atoi(argv[2]);
	
	int rank,size;

    x = new long long int[n];
    
    rand_fill_values();

    MPI_Init (&argc, &argv);
	
	MPI_Barrier(MPI_COMM_WORLD);	
    std::chrono::time_point<std::chrono::system_clock> start_time=std::chrono::system_clock::now(); 

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
	
    s = sqrt(size);
	Dense_Block(rank);

    MPI_Finalize();

	if(rank==0){
    	std::chrono::time_point<std::chrono::system_clock> end=std::chrono::system_clock::now();
    	std::chrono::duration<double> elapsed_seconds = end-start_time;
    	std::cerr<<elapsed_seconds.count()<<std::endl;
	}
    
    return 0;
}
