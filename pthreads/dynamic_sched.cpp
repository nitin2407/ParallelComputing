#include <iostream>
#include <pthread.h>
#include <cmath>
#include <cstdlib>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

using namespace std;

/*
Declaring mutex
*/
pthread_mutex_t next_range_mutex;
pthread_mutex_t sum_mutex;

/*
declaring global variables which will be used by integrating functions also
*/
float a,b;
long intensity;
int function,n,granularity,thread_count;
int limit=0;

/*
declaring integrate functions
*/
void* itr_integrate(void* p);
void* th_integrate(void* p);
void* chnk_integrate(void* p);
float parallel_integrate (int argc, char* argv[]);

/*done and getNext functions to handle dynamic scheduler*/
bool done();
void getNext(int*,int*);

/*main function*/
int main (int argc, char* argv[]) {

  /*checking for available argumengts*/
  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  /*saving the start time*/
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  /*calling integrate function*/
  float integrate = parallel_integrate(argc, argv);

  /*saving the end time*/
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  /*caculating the totzal time taken*/
  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<integrate<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}

/*
parallel integrate function is the function which will initialize threads
and distrinbute cumputing amongst threads
*/
float parallel_integrate (int argc, char* argv[]){

  /*getting all input vales from arguments*/
  function = atoi(argv[1]);
  a = atof(argv[2]); 
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atol(argv[5]); 
  granularity = atoi(argv[8]);
  thread_count = atoi(argv[6]);
  string sync = argv[7];

  /*initializing mutex*/
  pthread_mutex_init (&next_range_mutex, NULL);
  pthread_mutex_init (&sum_mutex, NULL);
 
  pthread_t th[thread_count];
  
  float g_sum=0.0;

  /*loop to create threads and assign tasks*/  
  for(int j=0;j<thread_count;++j){
    if(sync=="iteration"){
        pthread_create(&th[j],NULL,itr_integrate, &g_sum);
    }else if(sync=="thread"){
        pthread_create(&th[j],NULL,th_integrate, &g_sum);  
    }else if(sync=="chunk"){
        pthread_create(&th[j],NULL,chnk_integrate, &g_sum);    
    }else{
        cerr<<"invalid sync type selection"<<endl;
    }
  }

  /*waiting for threads to complete*/
  for(int i=0;i<thread_count;i++){
    pthread_join(th[i],NULL);  
  }

  /*destroy mutex*/
  pthread_mutex_destroy (&next_range_mutex);
  pthread_mutex_destroy (&sum_mutex);

  return g_sum;

}

/*iteration level mutual exclusion*/
void* itr_integrate(void* p) {
  
  float temp = (b-a)/n;
  float integrate_out;
  
  /*checking if the computation is complete using 'done' function*/
  while(!done()){

      int start,stop;
      /*getting the values of begin and end of current computation*/
      getNext(&start,&stop);
      float *g_sum = (float *) p;
      
      /*loop to calculate integration*/
      for(int i=start;i<=stop;i++){
          switch(function){
            case 1:       
              integrate_out = f1(a+((i+0.5)*temp),intensity)*temp;    
              break;
            case 2:
              integrate_out = f2(a+((i+0.5)*temp),intensity)*temp; 
              break;
            case 3:
              integrate_out = f3(a+((i+0.5)*temp),intensity)*temp;    
              break;
            default:
              integrate_out = f4(a+((i+0.5)*temp),intensity)*temp;
        }

        /*locking mutex for each iteration*/
        pthread_mutex_lock (&sum_mutex); 
        *g_sum += integrate_out;
        pthread_mutex_unlock (&sum_mutex);
      }
  }
  return NULL;
}

/*chunk level mutual exclusion*/
void* chnk_integrate(void* p) {

  float temp = (b-a)/n;
  float func_param;

  /*checking if the computation is complete using 'done' function*/
  while(!done()){
    int start,stop;
    /*getting the values of begin and end of current computation*/
    getNext(&start,&stop);
    float sum = 0;
    float *g_sum = (float *) p;

    /*loop to calculate integration*/
    for(int i=start;i<=stop;i++){
      func_param = a+((i+0.5)*temp); 
      switch(function){
        case 1:    
          sum += (f1(func_param,intensity)*temp);   
          break; 
        case 2:
          sum += (f2(func_param,intensity)*temp);
          break;  
        case 3:
          sum += (f3(func_param,intensity)*temp);  
          break;   
        default:
          sum += (f4(func_param,intensity)*temp);
      }
    }

    /*locking mutex once for each chunk in thread*/
    pthread_mutex_lock (&sum_mutex);
    *g_sum = *g_sum + sum;  
    pthread_mutex_unlock (&sum_mutex);
  }
  return NULL;
}

/*thread level mutual exclusion*/
void* th_integrate(void* p) {

  float *g_sum = (float *) p;
  float temp = (b-a)/n;
  float func_param;
  float sum = 0;

  /*checking if the computation is complete using 'done' function*/
  while(!done()){
    int start,stop;
    /*getting the values of begin and end of current computation*/
    getNext(&start,&stop);
    
    /*loop to calculate integration*/
    for(int i=start;i<=stop;i++){
      func_param = a+((i+0.5)*temp); 
      switch(function){
        case 1:    
          sum += (f1(func_param,intensity)*temp);   
          break; 
        case 2:
          sum += (f2(func_param,intensity)*temp);
          break;  
        case 3:
          sum += (f3(func_param,intensity)*temp);  
          break;   
        default:
          sum += (f4(func_param,intensity)*temp);
      }
    }
  }

  /*locking mutex once for each thread*/
  pthread_mutex_lock (&sum_mutex);
  *g_sum = *g_sum + sum;  
  pthread_mutex_unlock (&sum_mutex);
  
  return NULL;
}

/*function to check if the computation is complete*/
bool done(){
  if(limit==-1){
    return true;
  }else{
    return false;
  }
}

/*function to get the next*/  
void getNext(int* start,int* stop){

  /*locking the mutex to find begin and end*/
  pthread_mutex_lock (&next_range_mutex);
  if(!done()){
    *start=limit;
    limit+=granularity;
    if(limit<n){
      *stop=limit-1;  
    }else{
      *stop=n-1;
      limit=-1;
    }
  }else{
    *start=0;
    *stop=-1;
  }
  pthread_mutex_unlock (&next_range_mutex);
}
