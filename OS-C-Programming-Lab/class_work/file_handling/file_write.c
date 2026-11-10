/**
 * @file file_write.c
 * @brief Demonstration of file creation and formatted writing in C.
 *
 * Concepts Demonstrated:
 *  - Creating or truncating files using fopen() with mode "w"
 *  - Formatted file stream output via fprintf() and fputs()
 *  - Stream buffer flushing and graceful closure using fclose()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("        File Stream Writing: file_write.c           \n");
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

    printf("[1] Opening destination file: \"%s\" in write mode (\"w\")...\n", filepath);
    printf("    Note: Mode \"w\" creates a new file or truncates an existing file.\n");

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("[Error] fopen failed");
        return EXIT_FAILURE;
    }

    printf("    File opened successfully for writing.\n\n");

    printf("[2] Writing records to file stream...\n");

    fputs("# Operating Systems Lab Records\n", file);
    fputs("----------------------------------------\n", file);

    // Formatted output
    fprintf(file, "Process ID  : %d\n", 1001);
    fprintf(file, "Module Name : %s\n", "Process Scheduler Simulation");
    fprintf(file, "Execution   : Successful\n");
    fprintf(file, "CPU Slices  : %d\n", 4);
    fprintf(file, "----------------------------------------\n");

    printf("    Records written successfully.\n\n");

    printf("[3] Closing file stream via fclose()...\n");
    if (fclose(file) != 0) {
        perror("[Error] fclose failed");
        return EXIT_FAILURE;
    }

    printf("    File stream flushed to disk and closed.\n");
    printf("    You can verify contents using: cat %s\n", filepath);
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
