/**
 * @file file_read.c
 * @brief Demonstration of file stream reading in C using fopen() and fgets().
 *
 * Concepts Demonstrated:
 *  - Standard C buffered I/O stream (FILE*)
 *  - Defensive handling of non-existent or inaccessible files
 *  - Sequential line-by-line reading using fgets() and ferror() verification
 *  - Safe stream closure via fclose()
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("        File Stream Reading: file_read.c            \n");
    printf("====================================================\n\n");

    const char *filepath = NULL;

    if (argc > 1) {
        if (argv[1][0] == '\0') {
            fprintf(stderr, "[Error] File path argument cannot be empty.\n");
            return EXIT_FAILURE;
        }
        filepath = argv[1];
    } else {
        // Check relative paths from inside OS-C-Programming-Lab or from parent dir
        FILE *probe = fopen("class_work/file_handling/sample_input.txt", "r");
        if (probe != NULL) {
            fclose(probe);
            filepath = "class_work/file_handling/sample_input.txt";
        } else {
            probe = fopen("OS-C-Programming-Lab/class_work/file_handling/sample_input.txt", "r");
            if (probe != NULL) {
                fclose(probe);
                filepath = "OS-C-Programming-Lab/class_work/file_handling/sample_input.txt";
            } else {
                filepath = "sample_input.txt";
            }
        }
    }

    printf("[1] Attempting to open file: \"%s\" in read mode (\"r\")...\n", filepath);

    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        perror("[Error] Failed to open file");
        fprintf(stderr, "Hint: Ensure \"%s\" exists or provide a valid file path as an argument.\n", filepath);
        return EXIT_FAILURE;
    }

    printf("    File opened successfully! Reading contents line by line:\n\n");
    printf("-------------------- FILE CONTENTS --------------------\n");

    char buffer[256];
    int line_number = 1;

    // fgets safely reads up to sizeof(buffer) - 1 bytes until newline or EOF
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("  Line %02d: %s", line_number++, buffer);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] != '\n') {
            printf("\n");
        }
    }

    if (ferror(file)) {
        perror("[Error] I/O error occurred while reading file");
        fclose(file);
        return EXIT_FAILURE;
    }

    printf("-------------------------------------------------------\n\n");

    printf("[2] Closing file stream with fclose()...\n");
    if (fclose(file) != 0) {
        perror("Failed to close file");
        return EXIT_FAILURE;
    }

    printf("    File stream closed cleanly.\n");
    printf("====================================================\n");

    return EXIT_SUCCESS;
}
