#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif
using namespace std;

void mergesort(int *a, int start, int end);
void merge(int *a, int start, int end);

int main (int argc, char* argv[]) {

  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int *temp = new int[n];
  int * arr = new int [n];

  generateMergeSortData (arr, atoi(argv[1]));

   // setting number of threads
   omp_set_num_threads(nbthreads);

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


   std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
   // parallel contsruct
    #pragma omp parallel
    {
        #pragma omp single
        {
        mergesort(arr, 0, n-1);
        }
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;

  checkMergeSortResult (arr, atoi(argv[1]));

  delete[] arr;

  return 0;
}


void merge(int *a, int start, int end)
{
    int mid=(start+end)/2;
    int index=0;

    int * temp = new int[end-start+1];

    int left = start;
    int right = mid+1;

    while((left <= mid) && (right <= end)) {
        /* if the left half value is less than right */
        if (a[left] < a[right]) {
            /* take from left */
            temp[index] = a[left];
            index++;
            left++;
        }
        else {
            /* take from right */
            temp[index] = a[right];
            index++;
            right++;
        }
    }

    /* add the remaining elements from the left half */
    while(left <= mid) {
        temp[index] = a[left];
        index++;
        left++;
    }

    /* add the remaining elements from the right half */
    while(right <= end) {
        temp[index] = a[right];
        index++;
        right++;
    }

    /* move from temp array to the original array */
    for(int i = start; i <= end; i++) {
        a[i] = temp[i - start];
    }

    delete[] temp;
}


void mergesort(int *a, int start, int end)
{
    if (start < end && (end-start)>10000){
        int mid = (start+end)/2;
        #pragma omp task
        mergesort(a, start, mid);
        #pragma omp task
        mergesort(a, mid+1, end);

        #pragma omp taskwait
    }else if(start<end){
        int mid = (start+end)/2;
        mergesort(a, start, mid);
        mergesort(a, mid+1, end);
    }

   merge(a, start, end);
}
