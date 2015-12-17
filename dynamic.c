#include <stdio.h>
#include <mpi.h>
#include "utilities.h"
#include "graphLib.h"
#include "settings.h"

#define TERMINATION_SIGNAL 9999999

int rank, size;
MPI_Status stat;


void doGC();
void doMaster();
void doWorker();
void aggregrateResults();
void mark(int node);

int main() {
	initialize();
	readInput();

	doGC();
	return 0;
}

void doGC() {
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// broadcast graph, rootset, and visited
	MPI_Bcast(&nodes, MAX_NODES*MAX_CHILDREN, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&numNodes, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&rootset, MAX_NODES, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&numRoots, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&visited, MAX_NODES, MPI_INTEGER, 0, MPI_COMM_WORLD);
	MPI_Bcast(&results, MAX_NODES, MPI_INTEGER, 0, MPI_COMM_WORLD);

	if (!rank) {
		double startTime, stopTime;

		startTime = MPI_Wtime();
		doMaster();
		stopTime = MPI_Wtime();

		//printAlive();
		printf("%lf\n", stopTime-startTime);
	} else {
		doWorker();
	}

	MPI_Finalize();
	return;
}

void doMaster() {
	int i, currentRoot=0, pendingComputations=0, terminationSignal=TERMINATION_SIGNAL;

	// load processors with an initial task
	while (rootset[currentRoot] && currentRoot < size-1) {
		MPI_Send(&rootset[currentRoot], 1, MPI_INTEGER, currentRoot+1, 0, MPI_COMM_WORLD);
		pendingComputations++;
		currentRoot++;
	}

	// keep feeding as processors become idle
	while (rootset[currentRoot]) {
		MPI_Recv(&results, MAX_NODES, MPI_INTEGER, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat);
		MPI_Send(&rootset[currentRoot], 1, MPI_INTEGER, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
		aggregrateResults();
		currentRoot++;
	}

	// if I didn't use all nodes, kill unused off
	for (i=currentRoot; i<size; i++)
		MPI_Send(&terminationSignal, 1, MPI_INTEGER, i, 0, MPI_COMM_WORLD);

	// signal termination and collect remaining results
	while (pendingComputations--) {
		MPI_Recv(&results, MAX_NODES, MPI_INTEGER, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat);
		MPI_Send(&terminationSignal, 1, MPI_INTEGER, stat.MPI_SOURCE, 0, MPI_COMM_WORLD);
		aggregrateResults();
	}

	return;
}

void doWorker() {
	int root;

	while(1) {
		MPI_Recv(&root, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &stat); // receive work
		if (root == TERMINATION_SIGNAL)
			break;

		mark(root);
		MPI_Send(&visited, MAX_NODES, MPI_INTEGER, 0, 0, MPI_COMM_WORLD); // report comptutation
	}
}

void aggregrateResults() {
	int i;
	for (i=0; i<MAX_NODES; i++)
		visited[i] = visited[i] || results[i];
}

void mark(int node) {
	int i=0;

	visited[node] = 1;
	while (nodes[node][i]) {
		if (!visited[nodes[node][i]]) {
			mark(nodes[node][i]);
		}
		i++;
	}
}
