#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <string>

using namespace std;


void find_factorial(long n,int num_threads,double* factorial){
    
    double *local_fact;
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp single
        {
            local_fact = new double[num_threads];
            local_fact[0] = 1;
        }
        double local_prod = 1;
        #pragma omp for schedule(static) nowait
        for (int i=2; i<n; i++) {
            local_prod = local_prod*i;
            factorial[i] = local_prod;
        }
        local_fact[thread_id+1] = local_prod;
        #pragma omp barrier
        double offset = 1;
        for(int i=0; i<(thread_id+1); i++)
        {
            offset = offset * local_fact[i];
        } 
        #pragma omp for schedule(static)
        for(int i=1; i<n; i++)
        {   
            factorial[i] = factorial[i] * offset;
        } 
    }

    delete[] local_fact;
    return;
}   

int main(int argc, char* argv[]) {
    
    #pragma omp parallel
    {
        int fd = open (argv[0], O_RDONLY);
        if (fd != -1) {
            close (fd);
        }
        else {
            std::cerr<<"something is amiss"<<std::endl;
        }
    }

    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }

    long n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    double *factorial = new double[n];
    factorial[1] = 1;

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    find_factorial(n,num_threads,factorial);

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

    cout<<factorial[n-1]<<endl;
    delete[] factorial;
    return 0;
}
