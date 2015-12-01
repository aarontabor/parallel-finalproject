#!/bin/bash

outputDir='./graphs'


mkdir $outputDir
for size in 5 10 25 50 100 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000; do
#for size in 5 10 25 50 100 500 1000 2000 5000 10000 20000; do
	echo building graph of size: $size...
	python generate.py $size > $outputDir/$size.graph
done
