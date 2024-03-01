#Operating Systems Lab 4

hostd: hostd.c utility.c utility.h
	gcc hostd.c utility.c -o hostd
clean: hostd.o utility.o
	rm hostd.o utility.o hostd
