#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <string>

using namespace std;

int main(void) {
    int n = 10;
    int factorial[n];
    factorial[1] = 1;
    for (int i=2; i<=n; i++)
        factorial[i] = factorial[i-1]*i;

    for(int i=1; i<n; i++)
    {
        cout<<factorial[i]<<endl;
    }
}