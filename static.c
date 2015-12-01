#include <stdio.h>
#include <mpi.h>
#include "utilities.h"
#include "graphLib.h"
#include "settings.h"


int rank, size, rootsPerSlot;
MPI_Status stat;


void doGC();
void doMaster();
void doWorker();
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

	rootsPerSlot = numRoots / size;

	if (!rank) {
		double startTime, stopTime;

		startTime = MPI_Wtime();
		doMaster();
		stopTime = MPI_Wtime();

		printAlive();
		//printf("%lf\n", stopTime-startTime);
	} else {
		doWorker();
	}

	MPI_Finalize();
	return;
}

void doMaster() {
	int i, j, start, end;

	start = rank * rootsPerSlot;
	end = (rank+1) * rootsPerSlot;

	// do my portion
	for (i=start; i<end; i++)
		mark(rootset[i]);

	// do leftover
	i = size*rootsPerSlot;
	while(rootset[i] != 0)
		mark(rootset[i++]);

	// gather results
	for (i=1; i<size; i++) {
		MPI_Recv(&results, MAX_NODES, MPI_INTEGER, i, i, MPI_COMM_WORLD, &stat);
		for (j=0; j<MAX_NODES; j++)
			visited[j] = visited[j] || results[j];
	}

	return;
}

void doWorker() {
	int i, start, end;

	start = rank * rootsPerSlot;
	end = (rank+1) * rootsPerSlot;

	// do my portion
	for (i=start; i<end; i++)
		mark(rootset[i]);

	MPI_Send(&visited, MAX_NODES, MPI_INTEGER, 0, rank, MPI_COMM_WORLD);
	return;
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
