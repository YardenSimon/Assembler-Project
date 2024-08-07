/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"

#define MAX_FILENAME_LENGTH 256

/* Function to process a single input file */
static void process_file(const char *filename) {
    char am_filename[MAX_FILENAME_LENGTH];
    char *base_name;
    size_t base_name_length;

    /* Extract base name of the file (without .as extension) */
    base_name = strrchr(filename, '/');
    if (base_name == NULL) {
        base_name = (char *)filename;
    } else {
        base_name++;  /* Skip the '/' */
    }
    base_name_length = strlen(base_name) - 3;  /* Remove .as extension */

    /* Create .am filename */
    strncpy(am_filename, base_name, base_name_length);
    am_filename[base_name_length] = '\0';
    strcat(am_filename, ".am");

    printf("Processing file: %s\n", filename);

    /* Initialize macro storage */
    init_macros();

    /* Replace macros and create .am file */
    replace_macros(filename);

    /* Perform first pass on .am file */
    perform_first_pass(am_filename);

    /* Perform second pass on .am file */
    perform_second_pass(am_filename);

    printf("Assembly completed for file: %s\n", filename);

    /* Free allocated resources */
    free_macros();
    free_memory();
}

/* Main function of the assembler */
int main(int argc, char *argv[]) {
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file1.as> [input_file2.as ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        process_file(argv[i]);
    }

    return 0;
}