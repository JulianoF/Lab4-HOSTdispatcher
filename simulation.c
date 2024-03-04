#include "simulation.h"


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

ProcSimulation* popFromQueue(ProcSimulation **queue, int *size) {
    if (*size == 0) { //If queue is empty, return NULL
        return NULL;
    }

    //* Deref size int, and decrement it
    (*size)--;

    ProcSimulation* poppedElement = (ProcSimulation*)malloc(sizeof(ProcSimulation)); //Allocate Pop elm obj

    if (poppedElement == NULL) {
        fprintf(stderr, "Memory allocation failed for Popped Element...\n");
        exit(EXIT_FAILURE);
    }

    // Copy the last element to the temporary ProcSimulation
    *poppedElement = (*queue)[*size];

    //todo: shrink queue obj?

    return poppedElement;
}

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

    ProcSimulation *p1Queue = NULL;
    ProcSimulation *p2Queue = NULL;
    ProcSimulation *p3Queue = NULL;

    ProcSimulation *RealTimeQueue = NULL;
    int p1Count = 0, p2Count = 0, p3Count = 0, RealTimeCount = 0;


    int ACTIVE_TASKS_COUNT;

    // Sort jobDispatchList into queues based on priority levels
    for (int i = 0; i < *count; i++) {
        int pLevel = JobDispatchlist[i].priority.level; // Assuming priority level is accessed this way
        ACTIVE_TASKS_COUNT++;

        switch(pLevel) {
            case 0:
                addToQueue(&RealTimeQueue, &RealTimeCount, JobDispatchlist[i]);
                break;
            case 1:
                addToQueue(&p1Queue, &p1Count, JobDispatchlist[i]);
                break;
            case 2:
                addToQueue(&p2Queue, &p2Count, JobDispatchlist[i]);
                break;
            case 3:
                addToQueue(&p3Queue, &p3Count, JobDispatchlist[i]);
                break;
            default:
                fprintf(stderr, "Unknown priority level %d\n", pLevel);
                break;
        }
    }

    // Debug print to verify sorting
    printQueue(RealTimeQueue, RealTimeCount, "Real Time Queue");
    printQueue(p1Queue, p1Count, "Priority 1 Queue");
    printQueue(p2Queue, p2Count, "Priority 2 Queue");
    printQueue(p3Queue, p3Count, "Priority 3 Queue");


    //* [------------] Simulation Logic [-------------]

    int MILLISEC_COUNT = 0;

    while (ACTIVE_TASKS_COUNT > 0 ) { //* Keep going as long as their are Processes needing exec
        if (RealTimeCount > 0) {
            // Assume processJob is a function that processes one job from the queue
            //processJob(&RealTimeQueue, &RealTimeCount);
        } 
        MILLISEC_COUNT++;
        //printf("[MS: %d]\n", MILLISEC_COUNT);
        usleep(1000); // Wait for 1 millisecond before the next iteration
    }

    //* [----------------------------------------------]


    // Free the dynamically allocated memory
    free(JobDispatchlist);
    free(RealTimeQueue);

    free(p1Queue);
    free(p2Queue);
    free(p3Queue);

    return 0;
}