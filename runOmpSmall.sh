#!/bin/bash

export OMP_NUM_THREADS=$NSLOTS
./sharedMem-static < graphs/100000.graph
