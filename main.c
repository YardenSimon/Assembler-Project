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
    char am_filename[MAX_FILENAME_LENGTH];

    printf("Processing file: %s\n", filename);

    init_macros();

    replace_macros(filename);  /* This creates the .am file */

    /* Get the name of the .am file created by replace_macros */
    sprintf(am_filename, "%s.am", filename);

    perform_first_pass(am_filename);  /* Use .am file for first pass */

    perform_second_pass(am_filename);  /* Use .am file for second pass as well */

    printf("Assembly completed for file: %s\n", filename);

    free_macros();
    free_memory();
}