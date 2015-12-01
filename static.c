#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "utilities.h"
#include "settings.h"


char buffer[BUFFER_SIZE];
int nodes[MAX_NODES][MAX_CHILDREN], rootset[MAX_NODES], visited[MAX_NODES], results[MAX_NODES], numNodes, numRoots;

int rank, size, rootsPerSlot;
MPI_Status stat;


void initialize();
void readInput();
void doGC();
void doMaster();
void doWorker();
void mark(int node);
void printNodes();
void printRootset();
void printAlive();

int main() {
	initialize();
	readInput();

	doGC();
	return 0;
}

void initialize() {
	int i, j;

	for (i=0; i<MAX_NODES; i++) {
		rootset[i] = 0;
		visited[i] = 0;
		results[i] = 0;

		for (j=0; j<MAX_CHILDREN; j++)
			nodes[i][j] = 0;
	}
}

void readInput() {
	int i, j;
	char *token;

	scanf("%d\n", &numNodes);

	fgets(buffer, BUFFER_SIZE, stdin);
	token = strtok(buffer, " ");
	i = 0;
	while (token) {
		rootset[i++] = atoi(token);
		token = strtok(NULL, " ");
	}
	numRoots = i-1;

	for (i=1; i<=numNodes; i++) {
		fgets(buffer, BUFFER_SIZE, stdin);
		token = strtok(buffer, " ");
		j = 0;
		while (token) {
			nodes[i][j++] = atoi(token);
			token = strtok(NULL, " ");
		}
	}

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


void printNodes() {
	int i, j=0;

	for (i=1; i<=numNodes; i++) {
		j = 0;
		while (nodes[i][j])
			printf("%d ", nodes[i][j++]);
		printf("\n");
	}
}

void printRootset() {
	int i;
	while (rootset[i])
		printf("%d ", rootset[i++]);
	printf("\n");
}

void printAlive() {
	int i;
	for (i=1; i<=numNodes; i++)
		if (visited[i])
			printf("%d\n", i);
}
