CFLAGS = -Wall

all: utilities.o sequential static sharedMem-static

sequential: ; gcc -o sequential sequential.c utilities.o

static: ; mpicc -o static static.c utilities.o

sharedMem-static: ; gcc -openmp -o sharedMem-static sharedMem-static.c utilities.o

clean: ; rm -f sequential utilities.o static sharedMem-static

run: ; mpirun -np 2 static < graphs/5.graph
