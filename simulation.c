#include <stdlib.h>
#include <string.h>

#include "simulation.h"

#include <stdlib.h>
#include <string.h>

//* Priorities Definition

//sets a quantum for a given priority ptr

void setQuantum(Priority* priority) {
    switch (priority->level) {
        case 0:
            priority->quantum = -1; // Real-time Priority has no quantum (inf)
            break;
        case 1:
            priority->quantum = 20; // Highest CPU scheduling priority, smallest quantum size
            break;
        case 2:
            priority->quantum = 40;
            break;
        case 3:
            priority->quantum = 60; // Lowest CPU scheduling priority, longest quantum size
            break;
        default:
            priority->quantum = 0; //! Default case, ideally should not hit this
    }
}

// Function to read processes from a file and return an array of ProcSimulation.
ProcSimulation* readProcessesFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    char line[256];
    int tempCount = 0;

    // First, count the number of lines
    while (fgets(line, sizeof(line), file)) {
        tempCount++;
    }

    ProcSimulation* processes = (ProcSimulation*)malloc(tempCount * sizeof(ProcSimulation));
    if (!processes) {
        fclose(file);
        return NULL; // Failed to allocate memory.
    }

    fseek(file, 0, SEEK_SET);
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d, %d, %d, %d, %d, %d, %d, %d",
               &processes[i].arrival_time,
               &processes[i].priority.level,
               &processes[i].allocated_exec_time,
               &processes[i].proc_size_in_mem,
               &processes[i].io.N_printers,
               &processes[i].io.N_scanners,
               &processes[i].io.N_modems,
               &processes[i].io.N_CDs);

        // Set the correct quantum for the process based on its priority level
        setQuantum(&processes[i].priority);

        i++;
    }

    fclose(file);
    *count = tempCount; // Set the count for the caller.
    return processes;
}


//* ------------------- Queue Logic --------------------

void addToQueue(ProcSimulation **queue, int *size, ProcSimulation process) {
    ProcSimulation *temp = realloc(*queue, (*size + 1) * sizeof(ProcSimulation));
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    *queue = temp;
    (*queue)[*size] = process;
    (*size)++;
}

void printQueue(ProcSimulation *queue, int size, char *queueName) {
    printf(">> %s:\n", queueName);
    for (int i = 0; i < size; i++) {
        printf("[PD %d ]: Arrival Time: %d, Priority: %d, Exec Time: %d, Mem: %dMB, Printers: %d, Scanners: %d, Modems: %d, CDs: %d\n",
               i,
               queue[i].arrival_time,
               queue[i].priority.level,
               queue[i].allocated_exec_time,
               queue[i].proc_size_in_mem,
               queue[i].io.N_printers,
               queue[i].io.N_scanners,
               queue[i].io.N_modems,
               queue[i].io.N_CDs);
    }
}

//* ------------------------------------------------------


int simulateDispatcher(ProcSimulation* JobDispatchlist, int* count) {
    
    if (JobDispatchlist == NULL) {
        // Handle error if file reading fails
        fprintf(stderr, "Failed to read processes from file.\n");
        return EXIT_FAILURE;
    }

    ProcSimulation *UserJobQueue = NULL;
    ProcSimulation *RealTimeQueue = NULL;

    int UserJobCount = 0, RealTimeCount = 0;

    // Sort jobDispatchList into queues
    for (int i = 0; i < *count; i++) {
        if (JobDispatchlist[i].priority.level == 0) {
            addToQueue(&RealTimeQueue, &RealTimeCount, JobDispatchlist[i]);
        } else {
            addToQueue(&UserJobQueue, &UserJobCount, JobDispatchlist[i]);
        }
    }

    // Debug print to verify sorting
    printQueue(RealTimeQueue, RealTimeCount, "Real Time Queue");
    printQueue(UserJobQueue, UserJobCount, "User Job Queue");

    // Free the dynamically allocated memory
    free(JobDispatchlist);
    free(UserJobQueue);
    free(RealTimeQueue);

    return 0;
}