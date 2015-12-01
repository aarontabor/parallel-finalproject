#!/bin/bash

set -e

resultsDir="./results-seq.`date +%s`"

schedule() {
	np=$1 # not really needed, but whatever
	outputFileName=$2
	script=$3
	echo "qsub -l h_rt=00:01:00 -cwd -N $outputFileName -o $resultsDir/$outputFileName $script"
}

mkdir $resultsDir

for i in 1 2 3; do
	schedule 1 s-$i ./runSequentialSmall.sh
	schedule 1 l-$i ./runSequentialLarge.sh
done
