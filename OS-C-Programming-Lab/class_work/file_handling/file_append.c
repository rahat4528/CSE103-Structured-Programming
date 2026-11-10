/**
 * @file file_append.c
 * @brief Demonstration of non-destructive file appending using fopen() with mode "a".
 *
 * Concepts Demonstrated:
 *  - Opening a file in append mode ("a") to position the stream offset at EOF
 *  - Appending new log entries without overwriting existing data
 *  - Verifying the updated file by reading and echoing complete contents
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("        File Stream Appending: file_append.c        \n");
    printf("====================================================\n\n");

    const char *filepath = NULL;

    if (argc > 1) {
        filepath = argv[1];
    } else {
        struct stat st;
        if (stat("class_work/file_handling", &st) == 0) {
            filepath = "class_work/file_handling/demo_output.txt";
        } else if (stat("OS-C-Programming-Lab/class_work/file_handling", &st) == 0) {
            filepath = "OS-C-Programming-Lab/class_work/file_handling/demo_output.txt";
        } else {
            filepath = "demo_output.txt";
        }
    }

    printf("[1] Opening file \"%s\" in append mode (\"a\")...\n", filepath);
    FILE *file = fopen(filepath, "a");

    if (file == NULL) {
        perror("[Error] fopen in append mode failed");
        return EXIT_FAILURE;
    }

    // Generate a simple timestamp or sequence entry
    time_t now = time(NULL);
    char time_str[32];
    struct tm *tm_info = localtime(&now);
    if (tm_info != NULL) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        snprintf(time_str, sizeof(time_str), "Timestamp N/A");
    }

    printf("[2] Appending new log entry to \"%s\"...\n", filepath);
    fprintf(file, "[LOG ENTRY] Timestamp: %s | Action: Appending new OS record\n", time_str);

    if (fclose(file) != 0) {
        perror("[Error] fclose failed");
        return EXIT_FAILURE;
    }
    printf("    Entry appended and buffer flushed.\n\n");

    printf("[3] Reading and displaying updated file contents:\n");
    printf("---------------- UPDATED FILE CONTENTS ----------------\n");

    FILE *verify_file = fopen(filepath, "r");
    if (verify_file != NULL) {
        char buffer[256];
        int line_no = 1;
        while (fgets(buffer, sizeof(buffer), verify_file) != NULL) {
            printf("  [%02d] %s", line_no++, buffer);
        }
        fclose(verify_file);
    } else {
        perror("Unable to reopen file for verification");
    }
    printf("-------------------------------------------------------\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
