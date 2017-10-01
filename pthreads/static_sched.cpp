#include <iostream>
#include <pthread.h>

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

pthread_mutex_t mut;
  
int function;
float a;
float b;
int n;
float intensity;
int thread_count;
float global_sum;

void* itr_integrate(void* p);

void* th_integrate(void* p) {
  int* val = (int*) p;
  float temp = (b-a)/n;
  int nmin;
  int nmax;
  float func_param;
  float sum = 0.0;
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
  for(int i=nmin;i<nmax;i++){
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
  pthread_mutex_lock (&mut);
    global_sum = global_sum + sum;  
  pthread_mutex_unlock (&mut);
  return NULL;
}

  
int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  function = atoi(argv[1]);
  a = atof(argv[2]); 
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]); 
  thread_count = atoi(argv[6]);
  pthread_mutex_init (&mut, NULL);
  pthread_t th[thread_count];
  global_sum = 0;
  string sync = argv[7];
  for(int j=0;j<thread_count;++j){
    if(sync=="iteration"){
      int* val= new int(j);
      pthread_create(&th[j],NULL,itr_integrate, val);
    }else{
      int* val= new int(j);
      pthread_create(&th[j],NULL,th_integrate, val);     
    }
  }
  for(int i=0;i<thread_count;i++){
    pthread_join(th[i],NULL);  
  }

  std::cout<<global_sum<<std::endl;
  
  return 0;
}

void* itr_integrate(void* p) {
  int* val = (int*) p;
  float temp = (b-a)/n;
  int nmin;
  int nmax;
  float func_param;
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
  for(int i=nmin;i<=nmax;i++){
    func_param = a+((i+0.5)*temp);
    pthread_mutex_lock (&mut); 
    if(function==1){    
        global_sum = global_sum + (f1(func_param,intensity)*temp);    
    }else if(function==2){
        global_sum = global_sum + (f2(func_param,intensity)*temp);  
    }else if(function==3){
        global_sum = global_sum + (f3(func_param,intensity)*temp);     
    }else if(function==4){
        global_sum = global_sum + (f4(func_param,intensity)*temp);
    }
    pthread_mutex_unlock (&mut);
  }
  return NULL;
}
