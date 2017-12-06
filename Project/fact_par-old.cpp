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

int main(void) {
    long n = 100000000;
    double *factorial = new double[n];
    factorial[1] = 1;

    double *proda;

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    #pragma omp parallel
    {
        int ithread = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            proda = new double[nthreads];
            proda[0] = 1;
        }
        double prod = 1;
        #pragma omp for schedule(static) nowait
        for (long i=2; i<n; i++) {
            prod *= i;
            factorial[i] = prod;
        }
        proda[ithread+1] = prod;
        #pragma omp barrier
        double offset = 1;
        for(long i=0; i<(ithread+1); i++) offset *= proda[i];
        #pragma omp for schedule(static)
        for(long i=1; i<n; i++) factorial[i] *= offset;
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

    delete[] proda;

    cout<<factorial[n-1]<<endl;
    /*for(int i=1; i<n; i++)
    {
        cout<<factorial[i]<<endl;
    }*/
}