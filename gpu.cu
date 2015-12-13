#include <assert.h>
#include <stdio.h>

extern "C" {
#include "utilities.h"
#include "graphLib.h"
#include "settings.h"
}

// cuda enforces an upper limit of 512 threads per blocks 
#define THREADS_PER_BLOCK 500

// cuda enforces an upper limit of 65xxx blocks per each grid dimension
#define BLOCKS_PER_DIM 65000

#define TOTAL_LIVE_OBJECTS 5


__device__ int getThreadId() {
  return 
    threadIdx.x +
    blockIdx.x * THREADS_PER_BLOCK +
    blockIdx.y * BLOCKS_PER_DIM * THREADS_PER_BLOCK +
    blockIdx.z * BLOCKS_PER_DIM * BLOCKS_PER_DIM * THREADS_PER_BLOCK;
}

__device__ int totalMarked(int *visited) {
	int marked = 0, i;
	for (i=0; i<MAX_NODES; i++)
		if (visited[i])
			marked++;

	return marked;
}

__global__ void gcKernel(int numNodes, int *graph, int *visited) {
	int index = getThreadId();
	if (index > numNodes)
		return;

	int *node = graph + (index*MAX_CHILDREN);
	int numMarked=0, i;
	while (numMarked < TOTAL_LIVE_OBJECTS) { // hardcoded for now
		if (visited[index]) { // I've been visited
			i = 0;
			while (node[i]) // notify all my children
				visited[node[i++]] = 1;
			return; // I'm done now
		}
		numMarked = totalMarked(visited);
	}
	return;
}


void computeGridDimesions(int totalThreads, dim3 *dimensions);

int main() {
	double startTime, stopTime;
	int i, *graph_d, *visited_d;
	dim3 gridDimensions;
	int graphSize = sizeof(int)*MAX_NODES*MAX_CHILDREN, visitedSize = sizeof(int)*MAX_NODES;

	initialize();
	readInput();

	// initialize visited array with rootset
	i=0;
	while (rootset[i])
		visited[rootset[i++]] = 1;

	// allocate and load graph and visited arrays on gpu
	assert(cudaMalloc(&graph_d, graphSize) == cudaSuccess);
	assert(cudaMalloc(&visited_d, visitedSize) == cudaSuccess);
	assert(cudaMemcpy(graph_d, nodes, graphSize, cudaMemcpyHostToDevice) == cudaSuccess);
	assert(cudaMemcpy(visited_d, visited, visitedSize, cudaMemcpyHostToDevice) == cudaSuccess);
	
	computeGridDimesions(numNodes, &gridDimensions);

	// collect garbage
	currentTimestamp(&startTime);
	gcKernel<<<gridDimensions, THREADS_PER_BLOCK>>>(numNodes, graph_d, visited_d);
	currentTimestamp(&stopTime);

	// copy visited back
	assert(cudaMemcpy(visited, visited_d, visitedSize, cudaMemcpyDeviceToHost) == cudaSuccess);

	// free graph and visited arrays on gpu
	cudaFree(graph_d); 
	cudaFree(visited_d);

	printAlive();
	//printf("%lf\n", stopTime-startTime);
	return 0;
}

void computeGridDimesions(int totalThreads, dim3 *dimensions) {
  int remainingBlocks = ceil((double)totalThreads / (double)THREADS_PER_BLOCK);

  dimensions->x = min(remainingBlocks, BLOCKS_PER_DIM);
  remainingBlocks = ceil((double)remainingBlocks / BLOCKS_PER_DIM);

  dimensions->y = min(remainingBlocks, BLOCKS_PER_DIM);
  remainingBlocks = ceil((double)remainingBlocks / BLOCKS_PER_DIM);

  dimensions->z = min(remainingBlocks, BLOCKS_PER_DIM);
  remainingBlocks = ceil((double)remainingBlocks / BLOCKS_PER_DIM);

  assert(remainingBlocks == 1); // otherwise, our problem is too big for grid!
}
