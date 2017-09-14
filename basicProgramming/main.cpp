#include <iostream>
#include <cmath>

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

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int function = atoi(argv[1]);
  float a = atof(argv[2]); 
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]); 
  float sum=0;
  float temp;


  if(function==1){
    temp = (b-a)/n;
    for(int i=0;i<n;i++){
      sum = sum + (f1(a+((i+0.5)*temp),intensity)*temp);
    }
  }else if(function==2){
    temp = (b-a)/n;
    for(int i=0;i<n;i++){
      sum = sum + (f2(a+((i+0.5)*temp),intensity)*temp);
    }
  }else if(function==3){
    temp = (b-a)/n;
    for(int i=0;i<n;i++){
      sum = sum + (f3(a+((i+0.5)*temp),intensity)*temp);
    }
  }else if(function==4){
    temp = (b-a)/n;
    for(int i=0;i<n;i++){ 
      sum = sum + (f4(a+((i+0.5)*temp),intensity)*temp);
    }
  }

  cout<<sum<<endl;

  return 0;
}
