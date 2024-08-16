/* main.c */

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

void print_memory_after_first_pass(void);


/* Function to process a single input file */
static void process_file(const char *filename) {
    char am_filename[MAX_FILENAME_LENGTH];
    BaseFilename base_filename;

    printf("Processing file: %s\n", filename);

    /* Initialize macro storage */
    init_macros();

    /* Replace macros and create .am file */
    replace_macros(filename);

    base_filename = get_base_filename(filename);

    strncpy(am_filename, base_filename.name, base_filename.length);
    am_filename[base_filename.length] = '\0';
    strcat(am_filename, ".am");

    /* Perform first pass on .am file */
    perform_first_pass(am_filename);

    print_memory_after_first_pass();


    print_symbol_table();

    /* Perform second pass on .am file */
    perform_second_pass(am_filename);

    printf("Assembly completed for file: %s\n", filename);

    /* Free allocated resources */
    free(base_filename.name);
    free_macros();
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
    free_memory();
    free_symbol_table();
    return 0;
}

#include <stdio.h>

void print_memory_after_first_pass() {
    int i, j;

    for (i = 0; i < memory_size; i++) {
        /* Only print if the memory location is non-zero, to avoid printing uninitialized memory slots */
        if (memory[i] != 0) {
            /* Print the address and the encoded word in binary */
            printf("Address: %d, Encoded Word: ", i + 100);  // Assuming address starts at 100
            for (j = 14; j >= 0; j--) {
                printf("%d", (memory[i] >> j) & 1);
            }
            printf("\n");
        }
    }
}