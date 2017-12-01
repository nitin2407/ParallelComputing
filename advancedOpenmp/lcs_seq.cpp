#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <cstring>

using namespace std;

int** lcs; 
char* x_arr;
char* y_arr;

static const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";

void get_random_string(char* a,int length)  // Random string generator function.
{
    for(int i=0;i<length;i++){
        a[i]=alphanum[rand() % 26];
    }
    
}


int calculate_lcs( char *X, char *Y, int m, int n )
{
   for (int i=0; i<=m; i++)
   {
     for (int j=0; j<=n; j++)
     {
       if (i == 0 || j == 0)
         lcs[i][j] = 0;
       else if (X[i-1] == Y[j-1])
         lcs[i][j] = lcs[i-1][j-1] + 1;
       else
         lcs[i][j] = max(lcs[i-1][j], lcs[i][j-1]);
     }
    }

    return lcs[m][n];
} 

int main(int argc,char* argv[]){

    srand(time(NULL));

    if (argc < 2) {
        std::cerr<<"Usage: "<<argv[0]<<" <size>"<<std::endl;
        return -1;
    }

    int row = stoi(argv[1]);
    int col = row;

    char* x = new char[row];
    char* y = new char[col];

    get_random_string(x,row);
    get_random_string(y,col);

    int lcs_length = 0;

    lcs = new int*[row+1];
    for(int i=0;i<=row;i++){
        lcs[i] = new int[col+1];
    }
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    lcs_length = calculate_lcs(x,y,row,col);

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    cout<<lcs_length<<endl;

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;


    delete[] lcs;

    return 0;

}

