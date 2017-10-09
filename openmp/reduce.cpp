#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif

void generateReduceData (int* arr, size_t n);
void reduce_min(int* arr,int argc,char* argv[]);
int find_min(int first,int second);
  
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
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
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];

  generateReduceData (arr, atoi(argv[1]));
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    
  int min = reduce_min(arr,argc,argv);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<min<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  delete[] arr;

  return 0;
}


int reduce_min(int* arr,int argc,char* argv[]){

    int n = stoi(argv[1]);
    int thread_count = stoi(argv[2]);
    string sched = argv[3];
    int granularity = stoi(argv[4]);
    int min=n;
    omp_set_num_threads(num_proc);

    if(granularity==-1){
      if(sched == "static"){
        #pragma omp parallel for schedule(static)
        for(int i=0;i<n;i++){
          min=find_min(arr[i],min);
        }
      }else{
        #pragma omp parallel for schedule(dynamic)
        for(int i=0;i<n;i++){
          min=find_min(arr[i],min);
        }
      }
    }else{
      if(sched == "static"){
        #pragma omp parallel for schedule(static, granularity)
        for(int i=0;i<n;i++){
          min=find_min(arr[i],min);
        }
      }else{
        #pragma omp parallel for schedule(dynamic, granularity)
        for(int i=0;i<n;i++){
          min=find_min(arr[i],min);
        }
      }
    }

    return min;
}

int find_min(int first,int second){

  return first <= second ? first : second;

}

