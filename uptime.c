#include <stdio.h>

#include "uptime.h"

struct time get_uptime() {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        printf("error opening /proc/uptime\n");
    
        struct time ret = {0, 0, 0, 0};
        return ret;
    }

    double temp;
    fscanf(fp, "%lf", &temp);
    fclose(fp);
    
    unsigned long uptime = (unsigned long)temp;
    
    unsigned int d = uptime / 86400;
    unsigned int h = (uptime % 86400) / 3600;
    unsigned int m = (uptime % 3600) / 60;
    unsigned int s = uptime % 60;

    struct time ret = {d, h, m, s};
    return ret;
}
