#!/bin/sh

. ./params_matmul.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for POWER in ${POWERS};
do
    for N in ${NS};
    do	

	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/matmul_${N}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${N},POWER=${POWER},PROC=${PROC} ./run_matmul.sh
	    fi

	done

    done
done


#weak scaling

for POWER in ${POWERS};
do
    for N in ${ST_NS};
    do	

	for PROC in ${PROCS}
	do  
	    FILE=${RESULTDIR}/matmul_${N}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${N},POWER=${POWER},PROC=${PROC} ./run_matmul.sh
	    fi

	done

    done
done


