#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

using namespace std;

#define MAX_NUMBER 100
int n, power;
long long int *x;
int **A;

void rand_fill_values()
{
    for(int i = 0;i<n; i++)
    {
        x[i] = rand()%MAX_NUMBER + 1;
        for(int j = 0;j<n;j++)
        {
            A[i][j] = rand()%MAX_NUMBER + 1;
        }
    }
    return;
}

int main (int argc, char* argv[]) {

    int x_power = 0;
    srand(time(NULL));

    if(argc != 3)
    {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <power>"<<std::endl;
        return -1;
    }

    n = atoi(argv[1]);
    power = atoi(argv[2]);

    long long int *y = new long long int[n];
    x = new long long int[n];
    A = new int*[n];    

    for(int i = 0; i < n; i++)
    {
        A[i] = new int[n];
    }

    rand_fill_values();

    std::chrono::time_point<std::chrono::system_clock> start_time=std::chrono::system_clock::now(); 

    while(power--)
    {
         for(int i = 0;i<n;i++)
        {
            y[i] = 0;
            for(int j = 0;j<n;j++)
            {
                y[i] += A[i][j]*x[j]; 
            }
        }

        for(int i = 0;i<n;i++)
        {
            x[i] = y[i];
        }
    }

    std::chrono::time_point<std::chrono::system_clock> end=std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    
    return 0;
}
