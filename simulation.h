#include <stdio.h>

//TODO: Idk if we need this ExecSim struct tbh
//From Lab 4 doc: 2)
//* Process ID, priority, processor time remaining (in seconds), memory location. block size, and resources requested

typedef struct {
    
    int pid;

    //* Both of these in seconds
    int allocated_exec_time; //Burst Execution time Required 
    int remaining_exec_time; //Time Left for Exec 

    int priority;
    int memory_loc;
    int block_size;

    char* request_rsrc; //? This is a char buffer, as it might be an array... we'll see

} ExecSimulation;  //Execution of process Simulation


typedef struct {

    int N_printers;
    int N_scanners;
    int N_modems;
    int N_CDs;

} IOSim;


//<arrival time>, <priority>, <processor time>, <Mbytes>, <#printers>, <#scanners>, <#modems>, <#CDs>

typedef struct {

    int arrival_time;
    int priority;

    int allocated_exec_time; //Processor Time it wants (In seconds)
    int proc_size_in_mem; // In MB

    IOSim io;

} ProcSimulation; //Process Simulation

ProcSimulation* readProcessesFromFile(const char* filename, int* count);