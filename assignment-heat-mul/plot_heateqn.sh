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
	FILE=${RESULTDIR}/heateqn_seq_${N}_${POWER}
	if [ ! -f ${FILE} ]
	then
	    echo missing sequential result file "${FILE}". Have you run queue_heateqn_seq and waited for completion?
	fi

	seqtime=$(cat ${RESULTDIR}/heateqn_seq_${N}_${POWER})
	
	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/heateqn_${N}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing heateqn result file "${FILE}". Have you run queue_heateqn and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/heateqn_${N}_${POWER}_${PROC})
	    
	    echo ${PROC} ${seqtime} ${partime}
	done > ${RESULTDIR}/speedup_heateqn_ni_${N}_${POWER}


	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N} i=${POWER}'; plot '${RESULTDIR}/speedup_heateqn_ni_${N}_${POWER}' u 1:(\$2/\$3);"
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
	    
	    FILE=${RESULTDIR}/heateqn_${REALN}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		echo missing heateqn result file "${FILE}". Have you run queue_heateqn and waited for completion?
	    fi

	    partime=$(cat ${RESULTDIR}/heateqn_${REALN}_${POWER}_${PROC})
	    
	    echo ${PROC} ${partime}
	done > ${RESULTDIR}/time_heateqn_ni_${N}_${POWER}


	GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. n=${N} i=${POWER}'; plot '${RESULTDIR}/time_heateqn_ni_${N}_${POWER}' u 1:2;"
    done
done


gnuplot <<EOF
set terminal pdf
set output 'heateqn_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
