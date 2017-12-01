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


N="1000 10000 1000000 1000000000"
THREADS="16"

make reduce reduce_seq

for n in $N;
do
    for t in $THREADS;
    do
	./reduce $n $t >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}
    ./reduce_seq $n >/dev/null 2> ${RESULTDIR}/reduction_seq_${n}
    done
done
	     
for n in $N;
do
    echo $(cat ${RESULTDIR}/reduction_seq_${n}) \
	$(cat ${RESULTDIR}/reduction_${n}_16) \
    > ${RESULTDIR}/speedup_reduction_${n}
done   

   