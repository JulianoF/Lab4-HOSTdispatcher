#include <stdio.h>
#include <unistd.h> // For usleep
#include <stdlib.h>
#include <string.h>


//From Lab 4 doc: 2)



typedef struct {

    int N_printers;
    int N_scanners;
    int N_modems;
    int N_CDs;

} IOSim;

typedef struct {

    int level; //0 realtime, 1,2,3 prority lists
    int quantum; // Milliseconds

} Priority;




//* <arrival time>, <priority>, <processor time>, <Mbytes>, <#printers>, <#scanners>, <#modems>, <#CDs>

typedef struct {

    int arrival_time;
    Priority priority;

    int allocated_exec_time; //Processor Time it wants (In seconds)
    int proc_size_in_mem; // In MB

    IOSim io;

} ProcSimulation; //Process Simulation

ProcSimulation* readProcessesFromFile(const char* filename, int* count);

int simulateDispatcher(ProcSimulation* JobDispatchlist, int* count);