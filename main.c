#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>

#include "uptime.h"
#include "mem.h"
#include "cpu.h"
#include "process.h"

#define VERSION "1.1.2"

#define COLOR_RESET  "\x1b[0m"
#define ACCENT_COLOR "\x1b[38;2;203;166;247m"
#define SECONDARY_COLOR "\x1b[38;2;116;199;236m"

void handle_sigint(int sig) {
    (void)sig;

    printf("\033[?25h\n%sBye-bye!%s\n", ACCENT_COLOR, COLOR_RESET);
    exit(0);
}

void handle_sigwinch(int sig) {
    (void)sig;

    printf("\033[H\033[J");
}

int main(int argc, char *argv[]) {
    // SIGNAL HANDLING
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    signal(SIGWINCH, handle_sigwinch);

    // CLI ARGUMENTS
    bool loop = false;
    int opt;
    int num_proc = 15;

    while ((opt = getopt(argc, argv, "lvn:")) != -1) {
        switch (opt) {
            case 'n':
                num_proc = atoi(optarg);
                if (num_proc > 1024) num_proc = 1024;
                if (num_proc < 0) num_proc = 0;
                break;
            case 'l':
                loop = true;
                break;
            case 'v':
                printf("rstat %sv%s%s | by %sevr4%s\n", SECONDARY_COLOR, VERSION, COLOR_RESET, SECONDARY_COLOR, COLOR_RESET);
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-l] [-v] [-n num]\n", argv[0]);
                return 1;
        }
    }
    
    if (loop) {
        printf("\033[?25l");
        printf("\033[H\033[J");
    }

    do {
        if (loop) {
            printf("\033[H");
        }

        // HOST INFO
        
        struct utsname uname_buf;
        uname(&uname_buf); 

        char *shell = getenv("SHELL");
        char *desktop = getenv("XDG_CURRENT_DESKTOP");

        printf("%s%-8s%s | sys %s%s%s | release %s%s%s | name %s%s%s\n", SECONDARY_COLOR, "host", COLOR_RESET, ACCENT_COLOR, uname_buf.sysname, COLOR_RESET, ACCENT_COLOR, uname_buf.release, COLOR_RESET, ACCENT_COLOR, uname_buf.nodename, COLOR_RESET);
        printf("%s%-8s%s | shell %s%s%s running in %s%s%s env\n", SECONDARY_COLOR, "host", COLOR_RESET, ACCENT_COLOR, shell, COLOR_RESET, ACCENT_COLOR, desktop, COLOR_RESET);

        // UPTIME
        struct time uptime = get_uptime();
        
        printf("%s%-8s%s | ", SECONDARY_COLOR, "uptime", COLOR_RESET);
        if (uptime.d > 0) {
            printf("%s%d%s day%s and ", ACCENT_COLOR, uptime.d, COLOR_RESET, uptime.d == 1 ? "" : "s");
        }

        printf("%s%02d:%02d:%02d%s\n\n", ACCENT_COLOR, uptime.h, uptime.m, uptime.s, COLOR_RESET);

        // MEMORY
        struct memory mem = get_mem();
        printf("%s%-8s%s | used %s%5.2lf%s GB | free %s%5.2lf%s GB | total %s%d%s GB\n\n", SECONDARY_COLOR, "memory", COLOR_RESET, ACCENT_COLOR, (double)mem.used / 1024 / 1024, COLOR_RESET, ACCENT_COLOR, (double)mem.free / 1024 / 1024, COLOR_RESET, ACCENT_COLOR, (int)(mem.total / 1024 / 1024), COLOR_RESET);

        // CPU
        double cpu_temp = get_cpu_temp();

        double cpu_usage = get_cpu_usage();
        printf("%s%-8s%s | %s%.1lf%s°C | load [", SECONDARY_COLOR, "cpu", COLOR_RESET, ACCENT_COLOR, cpu_temp, COLOR_RESET);

        int width = 20;
        int filled = (int)((cpu_usage / 100.0) * width);

        if (filled < 0) filled = 0;
        if (filled > width) filled = width;

        for (int i = 0; i < width; i++) {
            if (i < filled) {
                printf(ACCENT_COLOR "#" COLOR_RESET);
            } else {
                printf("-");
            }
        }

        printf("] %s%.1f%% %s\n\n", ACCENT_COLOR, cpu_usage, COLOR_RESET);

        // PROCESSES
        struct process *processes = get_top_processes();
        
        // maybe table?
        // printf("         |    pid     |         name         |          memory using\n");
        // printf("------------------------------------------------------------------------------\n");
        for (int i = 0; i < num_proc; i++) {
            printf("%sproc #%-2d%s | pid %s%6d%s | name %s%15.15s%s | uses %s%4lu%s MB of memory ( %s%.1lf%% %s)\n", SECONDARY_COLOR, i+1, COLOR_RESET, ACCENT_COLOR, processes[i].pid, COLOR_RESET, ACCENT_COLOR, processes[i].name, COLOR_RESET, ACCENT_COLOR, processes[i].memory_usage, COLOR_RESET, ACCENT_COLOR, (double)processes[i].memory_usage / ((double)mem.total / 1024.0) * 100.0, COLOR_RESET);
        }

        if (loop) {
            usleep(500000);
        }
    } while (loop);
    printf("\033[?25h");

    return 0;
}
