/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"

/* Function prototype */
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

    /* Initialize macro storage */
    init_macros();

    /* Macro processing stage */
    replace_macros(filename);

    /* First pass */
    if (!perform_first_pass(filename)) {
        fprintf(stderr, "First pass failed for file: %s\n", filename);
        free_macros();
        return;
    }

    /* Second pass */
    if (!perform_second_pass(filename)) {
        fprintf(stderr, "Second pass failed for file: %s\n", filename);
        free_macros();
        cleanup_assembler();  /* This cleans up both first and second pass allocations */
        return;
    }

    printf("Assembly completed successfully for file: %s\n", filename);

    /* Clean up */
    free_macros();
    cleanup_assembler();  /* This cleans up both first and second pass allocations */
}