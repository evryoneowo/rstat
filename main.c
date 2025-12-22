#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "uptime.h"
#include "mem.h"
#include "cpu.h"

#define VERSION "1.0"

#define COLOR_RESET  "\x1b[0m"
#define ACCENT_COLOR "\x1b[38;2;203;166;247m"
#define SECONDARY_COLOR "\x1b[38;2;116;199;236m"

void handle_sigint(int sig) {
    (void)sig;

    printf("\033[?25h\n%sBye-bye!%s\n", ACCENT_COLOR, COLOR_RESET);
    exit(0);
}

int main(int argc, char *argv[]) {
    // SIGINT & SIGTERM HANDLING
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    // CLI ARGUMENTS
    bool loop = false;
    int opt;

    while ((opt = getopt(argc, argv, "hlv")) != -1) {
        switch (opt) {
            case 'l':
                loop = true;
                break;
            case 'v':
                printf("rstat v%s | by evr4\n", VERSION);
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-l] [-v]\n", argv[0]);
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

        // UPTIME
        struct time uptime = get_uptime();
        
        printf("%s%-7s%s | ", SECONDARY_COLOR, "uptime", COLOR_RESET);
        if (uptime.d > 0) {
            printf("%s%d%s day%s | ", ACCENT_COLOR, uptime.d, COLOR_RESET, uptime.d == 1 ? "" : "s");
        }

        printf("%s%02d:%02d:%02d%s\n\n", ACCENT_COLOR, uptime.h, uptime.m, uptime.s, COLOR_RESET);

        // MEMORY
        struct memory mem = get_mem();
        printf("%s%-7s%s | used %s%5.2lf%s GB | free %s%5.2lf%s GB\n\n", SECONDARY_COLOR, "memory", COLOR_RESET, ACCENT_COLOR, (double)mem.used / 1024 / 1024, COLOR_RESET, ACCENT_COLOR, (double)mem.free / 1024 / 1024, COLOR_RESET);

        // CPU
        double cpu_temp = get_cpu_temp();

        double cpu_usage = get_cpu_usage();
        printf("%s%-7s%s | %s%.1lf%s°C | [", SECONDARY_COLOR, "cpu", COLOR_RESET, ACCENT_COLOR, cpu_temp, COLOR_RESET);

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

        printf("] %s%.1f%% %s\n", ACCENT_COLOR, cpu_usage, SECONDARY_COLOR);

        if (loop) {
            usleep(100000);
        }
    } while (loop);
    printf("\033[?25h");

    return 0;
}
