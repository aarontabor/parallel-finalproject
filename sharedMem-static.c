#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "utilities.h"
#include "settings.h"


char buffer[BUFFER_SIZE];
int nodes[MAX_NODES][MAX_CHILDREN], rootset[MAX_NODES], visited[MAX_NODES], numNodes;


void initialize();
void readInput();
void doGC();
void mark(int node);
void printNodes();
void printRootset();
void printAlive();

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

void initialize() {
	int i, j;

	for (i=0; i<MAX_NODES; i++) {
		rootset[i] = 0;
		visited[i] = 0;

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
