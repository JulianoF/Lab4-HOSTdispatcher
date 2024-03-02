#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"

int main(void){
    int count;
    ProcSimulation* processes = readProcessesFromFile("dispatch_list_1", &count);

    // Example usage
    for (int i = 0; i < count; i++) {
        printf("Process %d: Arrival Time: %d, Priority: %d, Exec Time: %d, Mem: %dMB, Printers: %d, Scanners: %d, Modems: %d, CDs: %d\n",
               i,
               processes[i].arrival_time,
               processes[i].priority,
               processes[i].allocated_exec_time,
               processes[i].proc_size_in_mem,
               processes[i].io.N_printers,
               processes[i].io.N_scanners,
               processes[i].io.N_modems,
               processes[i].io.N_CDs);
    }

    free(processes);
    return 0;
}
