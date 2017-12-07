#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for POWER in ${POWERS};
do
    for N in ${NS};
    do
	FILE=${RESULTDIR}/matmul_seq_${N}_${POWER}
	if [ ! -f ${FILE} ]
	then
	    echo missing sequential result file "${FILE}". Have you run queue_matmul_seq and waited for completion?
	fi

	seqtime=$(cat ${RESULTDIR}/matmul_seq_${N}_${POWER})
	
	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/matmul_${N}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing matmul result file "${FILE}". Have you run queue_matmul and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/matmul_${N}_${POWER}_${PROC})
	    
	    echo ${PROC} ${seqtime} ${partime}
	done > ${RESULTDIR}/speedup_matmul_ni_${N}_${POWER}


	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N} i=${POWER}'; plot '${RESULTDIR}/speedup_matmul_ni_${N}_${POWER}' u 1:(\$2/\$3);"
    done
done

#weak scaling

for POWER in ${POWERS};
do
    for N in ${WK_NS};
    do
	
	for PROC in ${PROCS}
	do
	    REALN=$( echo ${N} \* ${PROC}  | bc)
	    
	    FILE=${RESULTDIR}/matmul_${REALN}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing matmul result file "${FILE}". Have you run queue_matmul and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/matmul_${REALN}_${POWER}_${PROC})
	    
	    echo ${PROC} ${partime}
	done > ${RESULTDIR}/time_matmul_ni_${N}_${POWER}


	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. n=${N} i=${POWER}'; plot '${RESULTDIR}/time_matmul_ni_${N}_${POWER}' u 1:2;"
    done
done


gnuplot <<EOF
set terminal pdf
set output 'matmul_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
