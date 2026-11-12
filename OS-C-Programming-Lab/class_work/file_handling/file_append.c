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
#include <string.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("        File Stream Appending: file_append.c        \n");
    printf("====================================================\n\n");

    const char *filepath = NULL;

    if (argc > 1) {
        if (argv[1][0] == '\0') {
            fprintf(stderr, "[Error] File path cannot be empty.\n");
            return EXIT_FAILURE;
        }
        filepath = argv[1];
    } else {
        struct stat st;
        if (stat("class_work/file_handling", &st) == 0 && S_ISDIR(st.st_mode)) {
            filepath = "class_work/file_handling/demo_output.txt";
        } else if (stat("OS-C-Programming-Lab/class_work/file_handling", &st) == 0 && S_ISDIR(st.st_mode)) {
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

    // Generate a formatted timestamp
    time_t now = time(NULL);
    char time_str[32];
    struct tm *tm_info = localtime(&now);
    if (tm_info != NULL) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        snprintf(time_str, sizeof(time_str), "Timestamp N/A");
    }

    printf("[2] Appending new log entry to \"%s\"...\n", filepath);
    if (fprintf(file, "[LOG ENTRY] Timestamp: %s | Action: Appending new OS record\n", time_str) < 0) {
        perror("[Error] fprintf failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (fclose(file) != 0) {
        perror("[Error] fclose failed");
        return EXIT_FAILURE;
    }
    printf("    Entry appended and buffer flushed.\n\n");

    printf("[3] Reading and displaying updated file contents:\n");
    printf("---------------- UPDATED FILE CONTENTS ----------------\n");

    FILE *verify_file = fopen(filepath, "r");
    if (verify_file == NULL) {
        perror("Unable to reopen file for verification");
        return EXIT_FAILURE;
    }

    char buffer[256];
    int line_no = 1;
    while (fgets(buffer, sizeof(buffer), verify_file) != NULL) {
        printf("  [%02d] %s", line_no++, buffer);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] != '\n') {
            printf("\n");
        }
    }

    if (ferror(verify_file)) {
        perror("[Error] Failed while reading back file");
        fclose(verify_file);
        return EXIT_FAILURE;
    }

    fclose(verify_file);
    printf("-------------------------------------------------------\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
