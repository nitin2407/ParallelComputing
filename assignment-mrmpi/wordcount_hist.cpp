#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mapreduce.h>
#include <keyvalue.h>
#include <sys/stat.h>

using namespace MAPREDUCE_NS;
using namespace std;

void map_task(int itask, char *file_name, KeyValue *kv, void *ptr)
{
    struct stat file_stat;
    int flag = stat(file_name,&file_stat);
    if (flag < 0) 
    {
        cerr<<"ERROR: File could not be read"<<endl;
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    int file_size = file_stat.st_size;

    FILE *fp = fopen(file_name,"r");
    char *text = new char[file_size+1];
    int nchar = fread(text,1,file_size,fp);
    text[nchar] = '\0';
    fclose(fp);
    char *tokens = " \'\".,?-!/#(){}[]:;<>\t\n\f\r\0";
    char *word = strtok(text,tokens);
    while (word) 
    {
        kv->add(word,strlen(word)+1,NULL,0);
        word = strtok(NULL,tokens);
    }
    delete [] text;
}

void reduce_task(char *key, int keybytes, char *multivalue, int nvalues, int *valuebytes, KeyValue *kv, void *ptr)
{
    kv->add((char *) &nvalues,sizeof(int),key,keybytes);
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <file1> <file2>..."<<std::endl;
        return -1;
    }

    MPI_Init(&argc,&argv);

    int rank,size;

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    MapReduce *mr = new MapReduce(MPI_COMM_WORLD);
    mr->verbosity = 2;

    MPI_Barrier(MPI_COMM_WORLD);

    int nwords = mr->map(argc-1,&argv[1],0,1,0,map_task,NULL);
    int nfiles = mr->mapfilecount;
    mr->collate(NULL);
    int nunique = mr->reduce(reduce_task,NULL);
    
    mr->collate(NULL);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    /*Printing to a file*/
    char *outp = new char[6];
    strncpy(outp, "histog", sizeof(outp)-1);
    mr->print(outp, 1, 0, 1, 5, 1);

    //mr->print(-1,1,1,5);

    MPI_Barrier(MPI_COMM_WORLD);

    delete mr;

    if (rank == 0) 
    {
        cout<<"Total words : "<<nwords<<", Unique words : "<<nunique<<endl;
    }

    MPI_Finalize();
}