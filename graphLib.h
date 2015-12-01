#ifndef __graphLib_h__
#define __graphLib_h__

#include "settings.h"

char buffer[BUFFER_SIZE];
int nodes[MAX_NODES][MAX_CHILDREN], rootset[MAX_NODES], numRoots, visited[MAX_NODES], results[MAX_NODES], numNodes;

void initialize();
void readInput();
void printNodes();
void printRootset();
void printAlive();

#endif

