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


N="1000 10000 100000 1000000"
THREADS="16"

make bubblesort bubblesort_seq

for n in $N;
do
    for t in $THREADS;
    do
	./bubblesort $n $t 1000 >/dev/null 2> ${RESULTDIR}/bubblesort_${n}_${t}
    ./bubblesort_seq $n >/dev/null 2> ${RESULTDIR}/bubblesort_seq_${n}
    done
done

for n in $N;
do
	#output in format "thread seq par"
	echo $(cat ${RESULTDIR}/bubblesort_seq_${n}) \
	     $(cat ${RESULTDIR}/bubblesort_${n}_16) \
    > ${RESULTDIR}/speedup_bubblesort_${n}
done
