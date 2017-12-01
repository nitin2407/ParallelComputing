#include <omp.h>
#include <iostream>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

static const char alphabets[] = "abcdefghijklmnopqrstuvwxyz";

void get_random_string(char* a,int length)  // Random string generator function.
{

    #pragma omp parallel for
    for(int i=0;i<length;i++){
        a[i]=alphabets[rand() % 26];
    }
    
}

void calculate_lcs_task(int i,int j,char* x,char* y,int** lcs){

	if (i == 0 || j == 0){
		lcs[i][j] = 0;
    }else if (x[i - 1] == y[j - 1]){
		lcs[i][j] = lcs[i - 1][j - 1] + 1;
    }else{
		lcs[i][j] = max(lcs[i - 1][j], lcs[i][j - 1]);
    }

    return;
}

int calculate_lcs(char* x,char* y,int row,int col,int** lcs,int gran){

    int j=0;
    int i=0;
    
    for (; j <= col && i <= row; i++)
    {
	
		int d_size = min(i,col-j);						
		#pragma omp parallel for schedule(dynamic,gran)
        //for each anti-diagonal element
		for(int k=0;k<=d_size;++k)		            
		{
            int x_index=i-k;
            int y_index=j+k;
            //cout<<k<<endl;
            //#pragma omp task
			calculate_lcs_task(x_index,y_index,x,y,lcs);
		}
        #pragma omp barrier
        //#pragma omp taskwait
		if(i>=row)								
			i=row-1,j++;
        
	}
    //cout<<"comepleted loop"<<endl;
	return lcs[row][col];

}

int main(int argc,char* argv[]){

    srand(time(NULL));

    if (argc < 4) {
        std::cerr<<"Usage: "<<argv[0]<<" <size> <nbthreads> <granularity>"<<std::endl;
        return -1;
    }

    int row = stoi(argv[1]);
    int col = row;

    char* x = new char[row];
    char* y = new char[col];

    get_random_string(x,row);
    get_random_string(y,col);

    /*for(int i=0;i<row;i++){
        cout<<x[i];
    }
    cout<<endl;

    for(int i=0;i<col;i++){
        cout<<y[i];
    }
    cout<<endl;*/

    int threads = stoi(argv[2]);
    int gran = stoi(argv[3]);

    
    if(gran<0){
        gran=1;
    }

    omp_set_num_threads(threads);

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

    int lcs_length =0;

    int** lcs = new int*[row+1];
    
    for(int i=0;i<=row;i++){
        lcs[i] = new int[col+1];
    }
    
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    

    /*for(int i =0;i<n;i++){

        cout<<arr[i]<<endl;
    }*/

    /*#pragma omp parallel
	{
		#pragma omp single
		{*/
	        lcs_length = calculate_lcs(x,y,row,col,lcs,gran);
	    //}
	//}

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    cout<<lcs_length<<endl;

    std::chrono::duration<double> elapsed_seconds = end-start;
  
    std::cerr<<elapsed_seconds.count()<<std::endl;


    delete[] lcs;

    return 0;

}

	

