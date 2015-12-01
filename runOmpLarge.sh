#!/bin/bash

export OMP_NUM_THREADS=$NSLOTS
./sharedMem-static < graphs/1000000.graph
