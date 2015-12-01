from sys import argv
from random import randint, sample
from math import ceil

# Output format:
# 	<number of nodes>
#		<root 1> <root 2> ...
# 	<node1-ref 1> <node1-ref 2> ...
# 	<node2-ref 1> <node2-ref 2> ...
#		...

PERCENT_ROOT = 0.01
MAX_CHILDREN = 5

if len(argv) < 2:
	print('usage: python generate.py <# of nodes>')
	exit(1)

n = int(argv[1])
nodes = set(range(1, n+1)) # node ID's begin at 1 (0 is reserved for empty)

numRoots = ceil(n*PERCENT_ROOT)
roots = sample(nodes, numRoots)

# number of nodes
print(n)

# roots 
for root in roots:
	print(root, end = ' ');
print()

# nodes 
for node in nodes:
	numChildren = randint(0, min(n-1, MAX_CHILDREN+1))
	children = sample(nodes-set([node]), numChildren)

	for child in children:
		print(child, end=' ')
	print()
