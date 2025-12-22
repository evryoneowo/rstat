#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process.h"

int comparator(const void *a, const void *b) {
    struct process *p1 = (struct process*)a;
    struct process *p2 = (struct process*)b;
    
    if (p1->memory_usage > p2->memory_usage) {
        return -1;
    } else if (p1->memory_usage < p2->memory_usage) {
        return 1;
    } else {
        return 0;
    }
}

struct process* get_top_processes() {
    static struct process processes[1024];
    unsigned short count = 0;

    DIR *proc = opendir("/proc/");
    
    struct dirent *entry;

    while ((entry = readdir(proc)) != NULL && count < 1024) {
        if (isdigit(entry->d_name[0])) {
            char comm_path[512], statm_path[512];
            snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);
            snprintf(statm_path, sizeof(statm_path), "/proc/%s/statm", entry->d_name);

            FILE *comm_fp = fopen(comm_path, "r");
            FILE *statm_fp = fopen(statm_path, "r");

            if (comm_fp && statm_fp) {
                char name[256];
                unsigned long rss = 0;

                if (fgets(name, sizeof(name), comm_fp)) {
                    name[strcspn(name, "\n")] = 0;
                }

                if (fscanf(statm_fp, "%*d %lu", &rss) == 1) {
                    unsigned long memory_mb = rss * sysconf(_SC_PAGESIZE) / 1024 / 1024;
                
                    processes[count].pid = atoi(entry->d_name);
                    processes[count].memory_usage = memory_mb;

                    strncpy(processes[count].name, name, sizeof(processes[count].name) - 1);
                    processes[count].name[sizeof(processes[count].name) - 1] = '\0';

                    count ++;
                }
            }

            if (comm_fp) fclose(comm_fp);
            if (statm_fp) fclose(statm_fp);
        }
    }

    closedir(proc);

    qsort(processes, count, sizeof(struct process), comparator);
    
    return processes;
}
