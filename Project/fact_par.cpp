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
    int n = 10;
    int factorial[n];
    factorial[1] = 1;

    int *proda;
    #pragma omp parallel
    {
        int ithread = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            proda = new int[nthreads];
            proda[0] = 1;
        }
        int prod = 1;
        #pragma omp for schedule(static) nowait
        for (int i=2; i<n; i++) {
            prod *= i;
            factorial[i] = prod;
        }
        proda[ithread+1] = prod;
        #pragma omp barrier
        int offset = 1;
        for(int i=0; i<(ithread+1); i++) offset *= proda[i];
        #pragma omp for schedule(static)
        for(int i=1; i<n; i++) factorial[i] *= offset;
    }
    delete[] proda;

    for(int i=1; i<n; i++)
    {
        cout<<factorial[i]<<endl;
    }
}