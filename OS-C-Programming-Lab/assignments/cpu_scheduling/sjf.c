/**
 * @file sjf.c
 * @brief Shortest Job First (SJF - Non-Preemptive) CPU Scheduling Simulation.
 *
 * Educational Simulation Note:
 *  - This is an educational simulation.
 *  - Assumption: All processes arrive at time zero (AT = 0).
 *  - Non-preemptive scheduling selects the available process with minimum burst time.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10
#define MAX_BURST_TIME 10000

typedef struct {
    int pid;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

// Overflow-safe comparator sorting ascending by burst time (ties broken by PID)
static int compare_burst(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    if (p1->burst_time != p2->burst_time) {
        return (p1->burst_time > p2->burst_time) - (p1->burst_time < p2->burst_time);
    }
    return (p1->pid > p2->pid) - (p1->pid < p2->pid);
}

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("     CPU Scheduling Simulation: SJF (Non-preemptive) \n");
    printf("====================================================\n");
    printf("[Note] Educational model. All processes arrive at time t = 0.\n\n");

    int use_default = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "--custom") == 0) {
            use_default = 0;
        } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage: %s [--custom | --help]\n", argv[0]);
            printf("  (no args) : Run verified benchmark dataset (P1=6, P2=8, P3=7, P4=3)\n");
            printf("  --custom  : Prompt for custom process count and burst times\n");
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "[Error] Unknown option: %s\n", argv[1]);
            fprintf(stderr, "Usage: %s [--custom | --help]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    int n = 4;
    Process proc[MAX_PROCESSES];

    if (use_default) {
        printf("[Mode] Running with standard educational test dataset:\n");
        proc[0] = (Process){.pid = 1, .burst_time = 6};
        proc[1] = (Process){.pid = 2, .burst_time = 8};
        proc[2] = (Process){.pid = 3, .burst_time = 7};
        proc[3] = (Process){.pid = 4, .burst_time = 3};
    } else {
        printf("Enter number of processes (1 - %d): ", MAX_PROCESSES);
        if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROCESSES) {
            fprintf(stderr, "[Error] Invalid process count (must be 1 - %d).\n", MAX_PROCESSES);
            return EXIT_FAILURE;
        }
        for (int i = 0; i < n; i++) {
            proc[i].pid = i + 1;
            printf("Enter Burst Time for Process P%d (1 - %d): ", proc[i].pid, MAX_BURST_TIME);
            if (scanf("%d", &proc[i].burst_time) != 1 ||
                proc[i].burst_time <= 0 || proc[i].burst_time > MAX_BURST_TIME) {
                fprintf(stderr, "[Error] Invalid burst time (must be 1 - %d).\n", MAX_BURST_TIME);
                return EXIT_FAILURE;
            }
        }
    }

    // Sort by burst time for SJF
    qsort(proc, (size_t)n, sizeof(Process), compare_burst);

    int current_time = 0;
    double total_wt = 0.0;
    double total_tat = 0.0;

    for (int i = 0; i < n; i++) {
        current_time += proc[i].burst_time;
        proc[i].completion_time = current_time;
        proc[i].turnaround_time = proc[i].completion_time; // AT = 0
        proc[i].waiting_time = proc[i].turnaround_time - proc[i].burst_time;

        total_wt += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
    }

    // Display ASCII Gantt Chart
    printf("\n--- GANTT CHART (Execution Order) ---\n|");
    for (int i = 0; i < n; i++) {
        printf("   P%d   |", proc[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < n; i++) {
        printf("       %2d", proc[i].completion_time);
    }
    printf("\n\n");

    // Display Results Table
    printf("----------------- SCHEDULING RESULTS -----------------\n");
    printf("| PID | Arrival | Burst Time | Completion | TAT | WT |\n");
    printf("|-----|---------|------------|------------|-----|----|\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d |    0    |     %2d     |     %2d     | %3d | %2d |\n",
               proc[i].pid, proc[i].burst_time, proc[i].completion_time,
               proc[i].turnaround_time, proc[i].waiting_time);
    }
    printf("------------------------------------------------------\n");

    printf("Average Waiting Time    : %.2f ms\n", total_wt / n);
    printf("Average Turnaround Time : %.2f ms\n", total_tat / n);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
