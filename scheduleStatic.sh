#!/bin/bash

set -e

resultsDir="./results-static.`date +%s`"

schedule() {
	np=$1
	outputFileName=$2
	script=$3
	qsub -l h_rt=00:01:00 -l h_stack=512M -cwd -pe ompi $np -N $outputFileName -o $resultsDir/$outputFileName $script
}

mkdir $resultsDir

for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
	for j in 1 2 3; do
		schedule $i s$i-$j ./runStaticSmall.sh
		schedule $i l$i-$j ./runStaticLarge.sh
	done
done
