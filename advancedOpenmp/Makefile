CFLAGS=-O3 -std=c11 -g -fopenmp
CXXFLAGS=-O3 -std=c++11 -g -fopenmp
LDFLAGS=-fopenmp
ARCHIVES=libintegrate.a libfunctions.a libgen.a 
LD=g++

all: reduce mergesort bubblesort bubblesort_seq reduce_seq lcs_par lcs_seq 

reduce: reduce.o
	$(LD) $(LDFLAGS) reduce.o $(ARCHIVES) -o reduce

reduce_seq: reduce_seq.o
	$(LD) $(LDFLAGS) reduce_seq.o $(ARCHIVES) -o reduce_seq

mergesort: mergesort.o
	$(LD) $(LDFLAGS) mergesort.o $(ARCHIVES) -o mergesort

bubblesort: bubblesort.o
	$(LD) $(LDFLAGS) bubblesort.o $(ARCHIVES) -o bubblesort

bubblesort_seq: bubblesort_seq.o
	$(LD) $(LDFLAGS) bubblesort_seq.o $(ARCHIVES) -o bubblesort_seq

lcs_par: lcs_par.o
	$(LD) $(LDFLAGS) lcs_par.o $(ARCHIVES) -o lcs_par

lcs_seq: lcs_seq.o
	$(LD) $(LDFLAGS) lcs_seq.o $(ARCHIVES) -o lcs_seq

libfunctions.a: functions.o
	ar rcs libfunctions.a functions.o

libintegrate.a: sequential_lib.o
	ar rcs libintegrate.a sequential_lib.o

libgen.a: gen_lib.o
	ar rcs libgen.a gen_lib.o

test_reduce: reduce
	./test_reduce.sh

clean:
	-rm *.o

