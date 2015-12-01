CFLAGS = -Wall

all: graphLib.o utilities.o sequential static sharedMem-static

sequential: ; gcc -o sequential sequential.c utilities.o graphLib.o

static: ; mpicc -o static static.c utilities.o graphLib.o

sharedMem-static: ; gcc -openmp -o sharedMem-static sharedMem-static.c utilities.o graphLib.o

clean: ; rm -f sequential utilities.o static sharedMem-static graphLib.o

run: ; mpirun -np 2 static < graphs/5.graph
