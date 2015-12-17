CFLAGS = -Wall

all: graphLib.o utilities.o sequential static dynamic sharedMem-static

sequential: ; gcc -o sequential sequential.c utilities.o graphLib.o

static: ; mpicc -o static static.c utilities.o graphLib.o

dynamic: ; mpicc -o dynamic dynamic.c utilities.o graphLib.o

sharedMem-static: ; gcc -fopenmp -o sharedMem-static sharedMem-static.c utilities.o graphLib.o

clean: ; rm -f sequential utilities.o static dynamic sharedMem-static graphLib.o

run: ; mpirun -np 2 dynamic < graphs/5.graph
