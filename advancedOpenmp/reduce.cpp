#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif

void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

using namespace std;

//int* thread_sum;

void find_thread_sum(int index,int* arr,int end,int* thread_sum){
    
    int t_index = omp_get_thread_num();
    int local_sum=0;
    
    for(int j=index;j<end;j++){
        local_sum += arr[j]; 
    }
    thread_sum[t_index] += local_sum;
}

void reduce(int n, int * arr,int gran,int* thread_sum){
    
    int index=0;
    while(index<n){
        int end = index+gran;
        if(end>n){
            end=n;
        }
        #pragma omp task
            find_thread_sum(index,arr,end,thread_sum);
        index+=gran;
    }
    #pragma omp taskwait
}

int main(int argc,char* argv[]){

    if (argc < 4) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <granularity>"<<std::endl;
        return -1;
    }

    int n = stoi(argv[1]);
    int threads = stoi(argv[2]);
    int gran = stoi(argv[3]);

    if(gran<0){
        gran=1;
    }

    if(gran>n){
        gran=n;
    }

    
    omp_set_num_threads(threads);

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

    int * arr = new int [n];
    
    generateReduceData (arr, n);

    int* thread_sum = new int[threads+1];

    for(int i=0;i<threads;i++){
        thread_sum[i] = 0;
    }

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    #pragma omp parallel 
	{
		#pragma omp single
		{
            //#pragma omp task
			reduce(n,arr,gran,thread_sum);
		}
	}
    //#pragma omp taskwait

    int total_sum = 0;

    for(int i=0;i<threads;i++){
        total_sum = total_sum + thread_sum[i];
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    cout<<total_sum<<endl;

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;

    delete[] arr;

    return 0;

}

