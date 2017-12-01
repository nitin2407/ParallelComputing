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

void bubbleSort(int* arr,int n)
{
   int i, j;
   for (i = 0; i < n-1; i++){       
       for (j = 0; j < n-i-1; j++){
           if (arr[j] > arr[j+1]){
               swap(arr[j], arr[j+1]);
           }
       }
   }
}


int main(int argc,char* argv[]){

    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
        return -1;
    }

    int n = stoi(argv[1]);

    int * arr = new int [n];
    
    generateMergeSortData (arr, n);
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    /*for(int i =0;i<n;i++){

        cout<<arr[i]<<endl;
    }*/
	bubbleSort(arr,n);

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    
    /*cout<<"sorted"<<endl;

    for(int i=0;i<n;i++){
        cout<<arr[i]<<endl;
    }*/

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;

    checkMergeSortResult (arr, n);

    delete[] arr;

    return 0;

}

