#include <stdio.h>
#include <string.h>

#include "mem.h"

struct memory get_mem() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("error opening file /proc/meminfo\n");
        
        struct memory mem = {0, 0};
        return mem;
    }

    char line[256];
    long free_mem, total_mem;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemAvailable", 12) == 0) {
            sscanf(line, "MemAvailable: %lu", &free_mem);
        }

        if (strncmp(line, "MemTotal", 8) == 0) {
            sscanf(line, "MemTotal: %lu", &total_mem);
        }
    }
    fclose(fp);

    struct memory mem = {total_mem - free_mem, free_mem};
    return mem;
}
