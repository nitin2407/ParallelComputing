CXX=mpicxx
LD=mpicxx
CXXFLAGS=-O3 -std=c++11
CFLAGS=-O3
ARCHIVES=libfunctions.a


heat_eqn_seq: heat_eqn_seq.o
	$(LD) $(LDFLAGS) heat_eqn_seq.o $(ARCHIVES) -o heat_eqn_seq

heat_eqn: heat_eqn.o
	$(LD) $(LDFLAGS) heat_eqn.o $(ARCHIVES) -o heat_eqn

heat_eqn_async: heat_eqn_async.o
	$(LD) $(LDFLAGS) heat_eqn_async.o $(ARCHIVES) -o heat_eqn_async

matmul_seq: matmul_seq.o
	$(LD) $(LDFLAGS) matmul_seq.o $(ARCHIVES) -o matmul_seq

matmul: matmul.o
	$(LD) $(LDFLAGS) matmul.o $(ARCHIVES) -o matmul

plots.pdf:
	./plots.sh
