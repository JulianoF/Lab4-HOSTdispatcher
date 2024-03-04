#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"


#define MAX_MEMORY_BUFFER_SIZE 1024 //In Bytes ofsource



int main(void) {

    int availableMemory = MAX_MEMORY_BUFFER_SIZE; //Init with max
    int count;

    ProcSimulation *JobDispatchlist = readProcessesFromFile("dispatch_list_1", &count);
    int sim = simulateDispatcher(JobDispatchlist, &count);
}


