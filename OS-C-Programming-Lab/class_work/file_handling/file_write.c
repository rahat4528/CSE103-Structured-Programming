/**
 * @file file_write.c
 * @brief Demonstration of file creation and formatted/block writing in C.
 *
 * Concepts Demonstrated:
 *  - Creating or truncating files using fopen() with mode "w"
 *  - Formatted file stream output via fprintf(), fputs(), and fwrite()
 *  - Checking write errors and graceful stream closure using fclose()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("        File Stream Writing: file_write.c           \n");
    printf("====================================================\n\n");

    const char *filepath = NULL;

    if (argc > 1) {
        if (argv[1][0] == '\0') {
            fprintf(stderr, "[Error] Output file path cannot be empty.\n");
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

    printf("[1] Opening destination file: \"%s\" in write mode (\"w\")...\n", filepath);
    printf("    Note: Mode \"w\" creates a new file or truncates an existing file.\n");

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("[Error] fopen failed");
        return EXIT_FAILURE;
    }

    printf("    File opened successfully for writing.\n\n");

    printf("[2] Writing records to file stream (fputs, fprintf, fwrite)...\n");

    if (fputs("# Operating Systems Lab Records\n", file) == EOF ||
        fputs("----------------------------------------\n", file) == EOF ||
        fprintf(file, "Process ID  : %d\n", 1001) < 0 ||
        fprintf(file, "Module Name : %s\n", "Process Scheduler Simulation") < 0 ||
        fprintf(file, "Execution   : Successful\n") < 0 ||
        fprintf(file, "CPU Slices  : %d\n", 4) < 0) {
        perror("[Error] Failed while writing formatted records to file");
        fclose(file);
        return EXIT_FAILURE;
    }

    const char *footer = "----------------------------------------\n";
    size_t footer_len = strlen(footer);
    if (fwrite(footer, sizeof(char), footer_len, file) != footer_len) {
        perror("[Error] fwrite failed while writing footer block");
        fclose(file);
        return EXIT_FAILURE;
    }

    printf("    Records written successfully (including %zu-byte fwrite block).\n\n", footer_len);

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
