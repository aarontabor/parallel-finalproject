CFLAGS = -Wall

all: graphLib.o utilities.o sequential static sharedMem-static gpu

sequential: ; gcc -o sequential sequential.c utilities.o graphLib.o

static: ; mpicc -o static static.c utilities.o graphLib.o

sharedMem-static: ; gcc -fopenmp -o sharedMem-static sharedMem-static.c utilities.o graphLib.o

gpu: ; nvcc -o gpu gpu.cu utilities.c graphLib.c

clean: ; rm -f sequential utilities.o static sharedMem-static graphLib.o gpu

run: ; mpirun -np 2 static < graphs/5.graph
