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


void find_factorial(long n,double* factorial){
    
    for (long i=2; i<=n; i++)
        factorial[i] = factorial[i-1]*i;

    return;
}   

int main(int argc, char* argv[]) {
   
    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
        return -1;
    }

    long n = atoi(argv[1]);

    double *factorial = new double[n];
    factorial[1] = 1;

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    find_factorial(n,factorial);

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

    cout<<factorial[n-1]<<endl;
    delete[] factorial;
    return 0;
}
