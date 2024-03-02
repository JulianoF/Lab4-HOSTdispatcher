#include <stdlib.h>
#include <string.h>

#include "simulation.h"

// Function to read processes from a file and return an array of ProcSimulation.
ProcSimulation* readProcessesFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Temporary storage for reading file lines.
    char line[256];
    int tempCount = 0;

    // First, count the number of lines to determine how many structures we need.
    while (fgets(line, sizeof(line), file)) {
        tempCount++;
    }

    // Allocate memory for the array of ProcSimulation structures.
    ProcSimulation* processes = (ProcSimulation*)malloc(tempCount * sizeof(ProcSimulation));
    if (!processes) {
        fclose(file);
        return NULL; // Failed to allocate memory.
    }

    // Reset file pointer to the beginning of the file to read data.
    fseek(file, 0, SEEK_SET);
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        // Use sscanf to parse the line into the structure fields.
        sscanf(line, "%d, %d, %d, %d, %d, %d, %d, %d",
               &processes[i].arrival_time,
               &processes[i].priority,
               &processes[i].allocated_exec_time,
               &processes[i].proc_size_in_mem,
               &processes[i].io.N_printers,
               &processes[i].io.N_scanners,
               &processes[i].io.N_modems,
               &processes[i].io.N_CDs);
        i++;
    }

    fclose(file);
    *count = tempCount; // Set the count for the caller.
    return processes;
}
