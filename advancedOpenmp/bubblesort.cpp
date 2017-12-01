#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif

using namespace std;


void bubbleSort(int arr[], int n,int gran){
    int index=0;
    for(int i=0;i<n;i++){
        index=i%2;
        #pragma omp parallel for schedule(guided,gran)
        for(int j=index;j<n-1;j+=2){
            {
                if(arr[j]>arr[j+1]){
                    swap(arr[j],arr[j+1]);
                }
            }
        }
        #pragma omp barrier
    }
}


int main(int argc,char* argv[]){

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

    int * arr = new int [n];
    
    generateMergeSortData (arr, n);
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    

    /*for(int i =0;i<n;i++){

        cout<<arr[i]<<endl;
    }*/

    /*#pragma omp parallel
	{
		#pragma omp single
		{*/
			bubbleSort(arr,n,gran);
	    //}
	//}

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    
    //cout<<"sorted"<<endl;

    /*for(int i=0;i<n;i++){
        cout<<arr[i]<<endl;
    }*/

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;

    checkMergeSortResult (arr, n);

    delete[] arr;

    return 0;

}

