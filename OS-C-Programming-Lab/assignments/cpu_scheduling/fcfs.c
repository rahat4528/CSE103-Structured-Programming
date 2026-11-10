/**
 * @file fcfs.c
 * @brief First-Come, First-Served (FCFS) CPU Scheduling Simulation.
 *
 * Educational Simulation Note:
 *  - This is an educational algorithm simulation, not the Linux kernel CFS scheduler.
 *  - Assumption: All processes arrive at time zero (Arrival Time AT = 0).
 *
 * Calculations:
 *  - Completion Time (CT): Time when execution completes.
 *  - Turnaround Time (TAT): TAT = CT - AT = CT
 *  - Waiting Time (WT): WT = TAT - Burst Time (BT)
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10

typedef struct {
    int pid;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("     CPU Scheduling Simulation: FCFS (Non-preemptive) \n");
    printf("====================================================\n");
    printf("[Note] Educational model. All processes arrive at time t = 0.\n\n");

    int n = 4;
    Process proc[MAX_PROCESSES];

    // Check if running in automated/default demonstration mode
    int use_default = 1;
    if (argc > 1 && strcmp(argv[1], "--custom") == 0) {
        use_default = 0;
    }

    if (use_default) {
        printf("[Mode] Running with standard educational test dataset:\n");
        proc[0] = (Process){.pid = 1, .burst_time = 6};
        proc[1] = (Process){.pid = 2, .burst_time = 8};
        proc[2] = (Process){.pid = 3, .burst_time = 7};
        proc[3] = (Process){.pid = 4, .burst_time = 3};
    } else {
        printf("Enter number of processes (1 - %d): ", MAX_PROCESSES);
        if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROCESSES) {
            fprintf(stderr, "Invalid process count.\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < n; i++) {
            proc[i].pid = i + 1;
            printf("Enter Burst Time for Process P%d: ", proc[i].pid);
            if (scanf("%d", &proc[i].burst_time) != 1 || proc[i].burst_time <= 0) {
                fprintf(stderr, "Invalid burst time.\n");
                return EXIT_FAILURE;
            }
        }
    }

    // FCFS Computation
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
    printf("\n--- GANTT CHART ---\n|");
    for (int i = 0; i < n; i++) {
        printf("   P%d   |", proc[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < n; i++) {
        printf("        %2d", proc[i].completion_time);
    }
    printf("\n\n");

    // Display Scheduling Table
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
