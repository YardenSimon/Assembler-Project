#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "globals.h"
#include "encoder.h"
#include "errors.h"

char string_table[MAX_STRINGS][MAX_STRING_LENGTH];
int string_count = 0;
char current_filename[MAX_FILENAME_LENGTH] = {0};

/* Function to process a single input file */
static void process_file(const char *filename) {
    char am_filename[MAX_FILENAME_LENGTH];
    BaseFilename base_filename;
    int file_has_errors = 0;

    printf("Processing file: %s\n", filename);

    init_error_handling();

    /* Initialize macro storage */
    init_macros();

    /* Replace macros and create .am file */
    replace_macros(filename);

    /* Check for macro-related errors */
    if (has_errors()) {
        file_has_errors = 1;
        printf("Errors encountered during macro processing of %s. Continuing assembly.\n", filename);
	return;
    }

    base_filename = get_base_filename(filename);

    strncpy(am_filename, base_filename.name, base_filename.length);
    am_filename[base_filename.length] = '\0';
    strcat(am_filename, ".am");

    strncpy(current_filename, am_filename, MAX_FILENAME_LENGTH - 1);
    current_filename[MAX_FILENAME_LENGTH - 1] = '\0';

    init_encoded_data();
    /* Perform first pass on .am file */
    perform_first_pass(am_filename);

    if (has_errors()) {
        file_has_errors = 1;
        printf("Errors encountered during first pass of %s. Continuing to second pass.\n", filename);
    }

    add_encoded_data_to_memory();

    /* Perform second pass on .am file */
    perform_second_pass(am_filename);

    if (has_errors()) {
        file_has_errors = 1;
    }

    if (file_has_errors) {
        print_errors();
        printf("Assembly of %s completed with errors.\n", filename);
    } else {
        printf("Assembly completed successfully for file: %s\n", filename);
    }

    free_encoded_data();
    free(base_filename.name);
    free_macros();
    free_error_handling();
}

/* Main function of the assembler */
int main(int argc, char *argv[]) {
    int i;
    int overall_success = 1; /* Flag to track overall success */

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file1.as> [input_file2.as ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        process_file(argv[i]);
        if (has_errors()) {
            overall_success = 0;
        }
    }

    free_memory();
    free_symbol_table();

    if (!overall_success) {
        printf("Assembly completed with errors in one or more files.\n");
        return 1;
    } else {
        printf("Assembly completed successfully for all files.\n");
        return 0;
    }
}