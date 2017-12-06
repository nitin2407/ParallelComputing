CFLAGS=-O3 -std=c11 -g -fopenmp
CXXFLAGS=-O3 -std=c++11 -g -fopenmp
LDFLAGS=-fopenmp 
LD=g++

all: fact_par fact_seq

fact_seq: fact_seq.o
	$(LD) $(LDFLAGS) fact_seq.o -o fact_seq

fact_par: fact_par.o
	$(LD) $(LDFLAGS) fact_par.o -o fact_par


