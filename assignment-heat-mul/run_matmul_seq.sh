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
    

make matmul_seq

. ./params_matmul.sh

for power in $POWERS;
do
    for n in $NS;
    do
	FILE=${RESULTDIR}/matmul_seq_${n}_${power}

	if [ ! -f ${FILE} ]
	then
	    ./matmul_seq ${n} ${power} 2>${RESULTDIR}/matmul_seq_${n}_${power}  >/dev/null
	fi
    done
done
