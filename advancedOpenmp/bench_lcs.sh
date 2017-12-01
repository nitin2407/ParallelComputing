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


N="1000 5000 10000 20000 50000 100000"
THREADS="16"

make lcs_par lcs_seq

for n in $N;
do
    for t in $THREADS;
    do
	./lcs_par $n $t 1000 >/dev/null 2> ${RESULTDIR}/lcs_${n}_${t}_par
    ./lcs_seq $n >/dev/null 2> ${RESULTDIR}/lcs_${n}_seq
    done
done

for n in $N;
do
	#output in format "thread seq par"
	echo $(cat ${RESULTDIR}/lcs_${n}_seq) \
	     $(cat ${RESULTDIR}/lcs_${n}_16_par) \
    > ${RESULTDIR}/speedup_lcs_${n}
done
