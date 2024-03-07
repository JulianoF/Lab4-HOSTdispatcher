#include "simulation.h"

//* Priorities Definition

// sets a quantum for a given priority ptr
void setQuantum(Priority *priority)
{
    switch (priority->level)
    {
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
ProcSimulation *readProcessesFromFile(const char *filename, int *count)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    char line[256];
    int tempCount = 0;

    // First, count the number of lines
    while (fgets(line, sizeof(line), file))
    {
        tempCount++;
    }

    ProcSimulation *processes = (ProcSimulation *)malloc(tempCount * sizeof(ProcSimulation));
    if (!processes)
    {
        fclose(file);
        return NULL; // Failed to allocate memory.
    }

    fseek(file, 0, SEEK_SET);
    int i = 0;
    while (fgets(line, sizeof(line), file))
    {
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

ProcSimulation *popFromQueue(ProcSimulation **queue, int *size)
{
    if (*size == 0)
    { // If queue is empty, return NULL
        return NULL;
    }

    //* Deref size int, and decrement it
    (*size)--;

    ProcSimulation *poppedElement = (ProcSimulation *)malloc(sizeof(ProcSimulation)); // Allocate Pop elm obj

    if (poppedElement == NULL)
    {
        fprintf(stderr, "Memory allocation failed for Popped Element...\n");
        exit(EXIT_FAILURE);
    }

    // Copy the last element to the temporary ProcSimulation
    *poppedElement = (*queue)[*size];

    // todo: shrink queue obj?

    return poppedElement;
}

void addToQueue(ProcSimulation **queue, int *size, ProcSimulation process)
{
    ProcSimulation *temp = realloc(*queue, (*size + 1) * sizeof(ProcSimulation));
    if (temp == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    *queue = temp;
    (*queue)[*size] = process;
    (*size)++;
}

void printQueue(ProcSimulation *queue, int size, char *queueName)
{
    printf(">> %s:\n", queueName);
    for (int i = 0; i < size; i++)
    {
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

void sortFCFSQueue(ProcSimulation *queue, int size)
{
    int i, j;
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (queue[j].arrival_time < queue[j + 1].arrival_time)
            {
                // Swap the processes
                ProcSimulation temp = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = temp;
            }
        }
    }
}

//* ------------------------------------------------------

void printWrap(int MS_COUNT, int plevel, int MEM_AVA, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Print the initial part of the message
    printf("[t= %dms ] -- Process Priority: %d -- [MEM AVA: %d ] -- ", MS_COUNT, plevel, MEM_AVA);

    // Print the custom part of the message
    vprintf(format, args);

    // Print the newline at the end
    printf("\n");

    va_end(args);
}

int simulateDispatcher(ProcSimulation *JobDispatchlist, int *count)
{
    if (JobDispatchlist == NULL)
    {
        fprintf(stderr, "Failed to read processes from file.\n");
        return EXIT_FAILURE;
    }

    // Initialize queues & queue sizes for each priority
    //* real-time (p0) count, p1 count, p2 count, p3 count

    ProcSimulation *queues[4] = {NULL, NULL, NULL, NULL};
    int queueCounts[4] = {0, 0, 0, 0};

    int MILLISEC_COUNT = 0;
    int ACTIVE_TASKS_COUNT = *count;

    int MEMORY_AVAILABLE = MEMORY_MAX;

    ProcSimulation *runningProcessPTR = NULL;
    int currentProcessIndex = -1; // -1 indicates no process is currently running

    while (ACTIVE_TASKS_COUNT > 0)
    {
        // Check for new arrivals and add them to the appropriate queue
        for (int i = 0; i < *count; i++)
        {
            if (JobDispatchlist[i].arrival_time == MILLISEC_COUNT)
            {
                int pLevel = JobDispatchlist[i].priority.level;

                MEMORY_AVAILABLE -= JobDispatchlist[i].proc_size_in_mem;

                addToQueue(&queues[pLevel], &queueCounts[pLevel], JobDispatchlist[i]);
                printWrap(MILLISEC_COUNT, pLevel, MEMORY_AVAILABLE, "Arrived !!!");
            }
        }

        // If there's no current process, find the next process to run based on priority
        if (runningProcessPTR == NULL)
        {
            for (int i = 0; i < 4; i++)
            {
                if (queueCounts[i] > 0)
                {
                    runningProcessPTR = popFromQueue(&queues[i], &queueCounts[i]);
                    currentProcessIndex = i;
                    printWrap(MILLISEC_COUNT, currentProcessIndex, MEMORY_AVAILABLE, "Starting Process with Priority %d", currentProcessIndex);
                    break;
                }
            }
        }

        if (runningProcessPTR != NULL)
        { //* Process Currently Executing!

            runningProcessPTR->allocated_exec_time--; // Decrement the remaining execution time

            printWrap(MILLISEC_COUNT, currentProcessIndex, MEMORY_AVAILABLE,
                      "Priority %d Process Running | Time Left: %dms", currentProcessIndex, runningProcessPTR->allocated_exec_time);

            // Check if the process has completed
            if (runningProcessPTR->allocated_exec_time <= 0)
            {

                MEMORY_AVAILABLE += runningProcessPTR->proc_size_in_mem;

                printWrap(MILLISEC_COUNT, currentProcessIndex, MEMORY_AVAILABLE,
                          "Priority %d Process Completed", currentProcessIndex);


                runningProcessPTR = NULL;
                ACTIVE_TASKS_COUNT--;
            }
            else if (runningProcessPTR->priority.quantum > 0)
            { // If not real-time, handle quantum
                runningProcessPTR->priority.quantum--;
                if (runningProcessPTR->priority.quantum == 0 && currentProcessIndex < 3)
                {

                    // Demote process to a lower priority queue if the quantum expires
                    printWrap(MILLISEC_COUNT, currentProcessIndex, MEMORY_AVAILABLE,
                              "Priority %d Process Time Slice Ended | Moving to Lower Priority Queue\n", currentProcessIndex);

                    runningProcessPTR->priority.level++;
                    setQuantum(&runningProcessPTR->priority);
                    addToQueue(&queues[runningProcessPTR->priority.level], &queueCounts[runningProcessPTR->priority.level], *runningProcessPTR);
                    runningProcessPTR = NULL; // Process demoted, no longer running
                }
            }
        }
        else
        { // idling state (waiting for tasks)
            printf("[t= %dms ] Idling ... [MEM AVA: %d]\n", MILLISEC_COUNT, MEMORY_AVAILABLE);
        }

        MILLISEC_COUNT++;
        usleep(1000); // Wait for 1 millisecond before the next iteration
    }

    for (int i = 0; i < 4; i++)
    {
        free(queues[i]);
    }

    return 0;
}
