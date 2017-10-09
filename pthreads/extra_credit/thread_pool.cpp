#include <iostream>
#include <pthread.h>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include<queue>

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
pthread_mutex_t integrate_mut;
pthread_mutex_t task_mut;
pthread_cond_t cond;
queue<int*> tasks;
/*
declaring global variables which will be used by integrating functions also
*/  
int function,thread_count,n;
float a,b,global_sum;
long intensity;

/*
declaring integrate functions
*/
void* itr_integrate(void* p);
void* th_integrate(void* p);
float parallel_integrate (int argc, char* argv[]);

/*function to submit work to threads*/
void submit_work(int* val);

/*main function*/  
int main (int argc, char* argv[]) {

  /*checking for available argumengts*/
  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
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

  /*getting all input values from arguments*/  
  function = atoi(argv[1]);
  a = atof(argv[2]); 
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atol(argv[5]); 
  thread_count = atoi(argv[6]);
  string sync = argv[7];

  /*initializing mutex*/
  pthread_mutex_init (&integrate_mut, NULL);
  pthread_mutex_init (&task_mut, NULL);
  pthread_cond_init(&cond, NULL);

  pthread_t th[thread_count];

  global_sum = 0;
  

  /*loop to create threadpool*/  
  for(int j=0;j<thread_count;++j){
    if(sync=="iteration"){
        pthread_create(&th[j],NULL,itr_integrate, NULL);
    }else if(sync=="thread"){
        pthread_create(&th[j],NULL,th_integrate, NULL);     
    }else{
        cerr<<"invalid sync type selection"<<endl;
    }
  }

  /*loop to assign tasks to threads*/
  for(int j=0;j<thread_count;++j){
    int* val= new int(j);
    submit_work(val);
  }

  /*waiting for threads to complete*/
  for(int i=0;i<thread_count;i++){
    pthread_join(th[i],NULL);  
  }

  /*destroy mutex*/
  pthread_mutex_destroy (&integrate_mut);
  pthread_mutex_destroy(&task_mut);

  return global_sum;

}

void submit_work(int* val){
    pthread_mutex_lock (&task_mut);
    tasks.push(val);
    pthread_mutex_unlock (&task_mut);
    pthread_cond_signal(&cond);
}

/*iteration level mutual exclusion*/
void* itr_integrate(void* p) {
    
    pthread_mutex_lock(&task_mut);
    while (tasks.empty()) {
            pthread_cond_wait(&cond, &task_mut);
        }
    int* val = tasks.front();
    tasks.pop();
    pthread_mutex_unlock(&task_mut);  

    float temp = (b-a)/n;
    int nmin;
    int nmax;
    float integrate_out;

    /*calculating the start and begin for each thread*/
    if(*val==0){
        nmin=0;
    }else{
        nmin=(n/thread_count)*(*val)+1;
    }
    if(*val==(thread_count-1)){
        nmax=n;
    }else{
        nmax=(n/thread_count)*((*val)+1);
    }
    
    
    /*loop to calculate integration*/
    for(int i=nmin;i<nmax;i++){
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
        pthread_mutex_lock (&integrate_mut); 
        global_sum += integrate_out;
        pthread_mutex_unlock (&integrate_mut);
    }
    return NULL;
}

/*thread level mutual exclusion*/
void* th_integrate(void *p) {

    pthread_mutex_lock(&task_mut);
    while (tasks.empty()) {
            pthread_cond_wait(&cond, &task_mut);
        }
    int* val = tasks.front();
    tasks.pop();
    pthread_mutex_unlock(&task_mut);  

    float temp = (b-a)/n;
    int nmin;
    int nmax;
    float func_param;
    float sum = 0.0;

    /*calculating the start and begin for each thread*/
    if(*val==0){
        nmin=0;
    }else{
        nmin=(n/thread_count)*(*val)+1;
    }
    if(*val==(thread_count-1)){
        nmax=n;
    }else{
        nmax=(n/thread_count)*((*val)+1);
    }

    /*loop to calculate integration*/
    for(int i=nmin;i<nmax;i++){
        func_param = a+((i+0.5)*temp); 
        switch(function){
            case 1:       
            sum+= (f1(func_param,intensity)*temp);    
            break;
            case 2:
            sum+= (f2(func_param,intensity)*temp);  
            break;
            case 3:
            sum+= (f3(func_param,intensity)*temp);     
            break;
            default:
            sum+= (f4(func_param,intensity)*temp);
        }

     }

    /*locking mutex for each thread*/
    pthread_mutex_lock (&integrate_mut);
        global_sum = global_sum + sum;  
    pthread_mutex_unlock (&integrate_mut);

    return NULL;
}
