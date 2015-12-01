#include <stdio.h>
#include <omp.h>
#include "utilities.h"
#include "graphLib.h"
#include "settings.h"


void doGC();
void mark(int node);


int main() {
	double startTime, stopTime;

	initialize();
	readInput();

	currentTimestamp(&startTime);
	doGC();
	currentTimestamp(&stopTime);

	printAlive();
	//printf("runtime: %lf\n", stopTime-startTime);
	return 0;
}

void doGC() {
	int i, numRoots=0;
	while (rootset[numRoots])
		numRoots++;

#pragma omp parallel for
	for (i=0; i<numRoots; i++)
		mark(rootset[i]);
}

void mark(int node) {
	int i=0;

	visited[node] = 1;
	while (nodes[node][i]) {
		if (!visited[nodes[node][i]])
			mark(nodes[node][i]);
		i++;
	}
}
