struct process {
    unsigned int pid;
    char name[256];
    unsigned long memory_usage;
};

struct process* get_top_processes();
