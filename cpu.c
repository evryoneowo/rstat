#include <stdio.h>
#include <unistd.h>

struct cpustat {
    unsigned long long idle;
    unsigned long long total;
};

struct cpustat get_cpustat() {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        struct cpustat ret = {0, 0};
        return ret;
    }

    char line[512];
    fgets(line, sizeof(line), fp);
    fclose(fp);

    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    unsigned long long current_idle = idle + iowait;
    unsigned long long current_active = user + nice + system + irq + softirq + steal;
    unsigned long long current_total = current_idle + current_active;

    struct cpustat ret = {current_idle, current_total};
    return ret;
}

static struct cpustat prev = {0, 0};
double get_cpu_usage() {
    static int initialized = 0;
    if (!initialized) {
        prev = get_cpustat();
        initialized = 1;
        usleep(500000);
    }
    struct cpustat current = get_cpustat();

    unsigned long long diff_total = current.total - prev.total;
    unsigned long long diff_idle = current.idle - prev.idle;

    prev = current;

    if (diff_total == 0) {
        return 0.0;
    }

    return (double)(diff_total - diff_idle) / diff_total * 100.0;
}

double get_cpu_temp() {
    FILE *fp = fopen("/sys/class/hwmon/hwmon1/temp1_input", "r");

    if (fp == NULL) {
        return -1;
    }

    double temp;
    fscanf(fp, "%lf", &temp);
    fclose(fp);

    return temp / 1000.0;
}
