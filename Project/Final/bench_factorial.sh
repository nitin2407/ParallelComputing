#!/bin/sh

RESULTDIR=result/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

N="1000 10000 100000 1000000 1000000000"
THREADS="1 2 4 8 12 16"

make fact_par fact_seq

for n in $N;
do
    ./fact_seq $n  >/dev/null 2> ${RESULTDIR}/fact_seq_${n}
    for t in $THREADS;
    do
	./fact_par $n $t  >/dev/null 2> ${RESULTDIR}/fact_par_${n}_${t}
    done
done

for n in $N;
do
    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/fact_seq_${n}) \
	     $(cat ${RESULTDIR}/fact_par_${n}_${t})
    done   > ${RESULTDIR}/speedup_factorial_${n}
done

gnuplot <<EOF

set terminal pdf
set output 'factorial_par_plots.pdf'

set style data linespoints


set key top left;
set xlabel 'threads';
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_factorial_${N}' u 1:(\$2/\$3) t 'factorial'


EOF
