#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphLib.h"
#include "settings.h"


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
