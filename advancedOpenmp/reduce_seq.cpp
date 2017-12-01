#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

using namespace std;

int reduce(int index, int * arr){
    if(index<0){
        return 0;
    }
    return arr[index] + reduce(index-1,arr); 
}

int main(int argc,char* argv[]){

    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
        return -1;
    }

    int n = stoi(argv[1]);;


    int * arr = new int [n];
    
    generateReduceData (arr, n);

    /*for(int i =0;i<n;i++){

        cout<<arr[i]<<endl;
    }*/

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    int sum = reduce(n-1,arr);    

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    cout<<sum<<endl;

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;

    return 0;

}

