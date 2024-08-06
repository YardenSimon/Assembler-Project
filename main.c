/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"

static void process_file(const char *filename);

int main(int argc, char *argv[]) {
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file1> [input_file2 ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        process_file(argv[i]);
    }

    return 0;
}

static void process_file(const char *filename) {
    printf("Processing file: %s\n", filename);

    init_macros();

    replace_macros(filename);

    if (!perform_first_pass(filename)) {
        fprintf(stderr, "First pass failed for file: %s\n", filename);
        free_macros();
        free_memory();
        return;
    }

    if (!perform_second_pass(filename)) {
        fprintf(stderr, "Second pass failed for file: %s\n", filename);
        free_macros();
        free_memory();
        return;
    }

    printf("Assembly completed successfully for file: %s\n", filename);

    free_macros();
    free_memory();
}