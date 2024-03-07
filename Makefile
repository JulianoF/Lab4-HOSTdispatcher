#Operating Systems Lab 4

hostd: hostd.c simulation.h simulation.c
	gcc hostd.c simulation.c -o hostd

debug: hostd.c simulation.h simulation.c
	clang -fsanitize=address -O1 -fno-omit-frame-pointer -g hostd.c simulation.c -o hostDEBUG

clean: hostd.o simulation.o
	rm hostd.o simulation.o hostd hostDEBUG
