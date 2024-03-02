#Operating Systems Lab 4

hostd: hostd.c utility.c utility.h simulation.h simulation.c
	gcc hostd.c utility.c simulation.c -o hostd
clean: hostd.o utility.o simulation.o
	rm hostd.o utility.o simulation.o hostd
