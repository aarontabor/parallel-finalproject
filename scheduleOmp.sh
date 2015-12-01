#!/bin/bash

set -e

resultsDir="./results-omp.`date +%s`"

schedule() {
	np=$1
	outputFileName=$2
	script=$3
	# reserve, as all these threads need to be on a single machine
	qsub -R y -l h_rt=00:01:00 -cwd -pe openmp $np -N $outputFileName -o $resultsDir/$outputFileName $script
}

mkdir $resultsDir

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15; do
	for j in 1 2 3; do
		schedule $i s$i-$j ./runOmpSmall.sh
		schedule $i l$i-$j ./runOmpLarge.sh
	done
done
