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

pthread_mutex_t mut1;
pthread_mutex_t mut2;

float a,b,intensity;
int function,n,granularity,thread_count;
int limit=0;

void* itr_integrate(void* p);
void* th_integrate(void* p);
void* chnk_integrate(void* p);
float parallel_integrate (int argc, char* argv[]);

bool done();
void getNext(int*,int*);

int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  float integrate = parallel_integrate(argc, argv);

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cout<<integrate<<std::endl;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}

float parallel_integrate (int argc, char* argv[]){

  function = atoi(argv[1]);
  a = atof(argv[2]); 
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]); 
  granularity = atoi(argv[8]);
  thread_count = atoi(argv[6]);
  string sync = argv[7];

  pthread_mutex_init (&mut1, NULL);
  pthread_mutex_init (&mut2, NULL);
  
  pthread_t th[thread_count];
  
  float g_sum=0.0;
    
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
  for(int i=0;i<thread_count;i++){
    pthread_join(th[i],NULL);  
  }

  pthread_mutex_destroy (&mut1);
  pthread_mutex_destroy (&mut2);

  return g_sum;

}

void* itr_integrate(void* p) {
  
  float temp = (b-a)/n;
  float func_param;
  while(!done()){
    int start,stop;
    getNext(&start,&stop);
    //cout<<start<<endl;
    //cout<<stop<<endl;
    float *g_sum = (float *) p;
    for(int i=start;i<=stop;i++){
      func_param = a+((i+0.5)*temp);
      pthread_mutex_lock (&mut1); 
      if(function==1){    
          *g_sum += (f1(func_param,intensity)*temp);    
      }else if(function==2){
          *g_sum += (f2(func_param,intensity)*temp);  
      }else if(function==3){
          *g_sum += (f3(func_param,intensity)*temp);     
      }else if(function==4){
          *g_sum += (f4(func_param,intensity)*temp);
      }
      pthread_mutex_unlock (&mut1);
    }
  }
  return NULL;
}

void* chnk_integrate(void* p) {

  float temp = (b-a)/n;
  float func_param;

  while(!done()){
    int start,stop;
    getNext(&start,&stop);
    float sum = 0;
    float *g_sum = (float *) p;
    for(int i=start;i<=stop;i++){
      func_param = a+((i+0.5)*temp); 
      if(function==1){    
          sum = sum + (f1(func_param,intensity)*temp);    
      }else if(function==2){
          sum = sum + (f2(func_param,intensity)*temp);  
      }else if(function==3){
          sum = sum + (f3(func_param,intensity)*temp);     
      }else if(function==4){
          sum = sum + (f4(func_param,intensity)*temp);
      }
    }
    pthread_mutex_lock (&mut1);
    *g_sum = *g_sum + sum;  
    pthread_mutex_unlock (&mut1);
  }
  return NULL;
}

void* th_integrate(void* p) {

  float *g_sum = (float *) p;
  float temp = (b-a)/n;
  float func_param;
  float sum = 0;
  while(!done()){
    int start,stop;
    getNext(&start,&stop);
    for(int i=start;i<=stop;i++){
      func_param = a+((i+0.5)*temp); 
      if(function==1){    
          sum = sum + (f1(func_param,intensity)*temp);    
      }else if(function==2){
          sum = sum + (f2(func_param,intensity)*temp);  
      }else if(function==3){
          sum = sum + (f3(func_param,intensity)*temp);     
      }else if(function==4){
          sum = sum + (f4(func_param,intensity)*temp);
      }

    }
  }
  pthread_mutex_lock (&mut1);
  *g_sum = *g_sum + sum;  
  pthread_mutex_unlock (&mut1);
  
  return NULL;
}

bool done(){
  //pthread_mutex_lock (&mut3);
  if(limit==-1){
    //pthread_mutex_unlock (&mut3);
    return true;
  }else{
    //pthread_mutex_unlock (&mut3);
    //return false;
    return false;
  }
}
  
void getNext(int* start,int* stop){

  pthread_mutex_lock (&mut2);
  *start=limit;

  limit+=granularity;
  if(limit<n){
     *stop=limit-1;  
  }else{
    *stop=n-1;
    limit=-1;
  }
  pthread_mutex_unlock (&mut2);
}
