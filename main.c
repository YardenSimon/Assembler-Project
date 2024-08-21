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

void print_memory_after_first_pass(void);

/* Function to process a single input file */
static void process_file(const char *filename) {
    char am_filename[MAX_FILENAME_LENGTH];
    BaseFilename base_filename;

    printf("Processing file: %s\n", filename);

    init_error_handling();

    /* Initialize macro storage */
    init_macros();

    /* Replace macros and create .am file */
    replace_macros(filename);

    /* Check for macro-related errors */
    if (has_errors()) {
        print_errors();
        printf("Errors encountered during macro processing of %s. Assembly aborted.\n", filename);
        free_macros();
        free_error_handling();
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
        print_errors();
        printf("Errors encountered during first pass of %s. Assembly aborted.\n", filename);
        free_encoded_data();
        free(base_filename.name);
        free_macros();
        return;
    }

    add_encoded_data_to_memory();
    print_memory_after_first_pass();
    /* Perform second pass on .am file */
    perform_second_pass(am_filename);

    if (has_errors()) {
        print_errors();
        printf("Errors encountered during assembly of %s\n", filename);
    } else {
        printf("Assembly completed successfully for file: %s\n", filename);
    }

    free_encoded_data();
    free(base_filename.name);
    free_macros();
}

/* Main function of the assembler */
int main(int argc, char *argv[]) {
    int i;
    printf("RUNNING MAIN");
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file1.as> [input_file2.as ...]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        process_file(argv[i]);
    }

    free_memory();
    free_symbol_table();

    if (has_errors()) {
        printf("Assembly completed with errors.\n");
        free_error_handling();
        return 1;
    } else {
        printf("Assembly completed successfully for all files.\n");
        free_error_handling();
        return 0;
    }
}

/* Debug function - can be removed in final version */
void print_memory_after_first_pass() {
    int i;
    MachineWord word;
    char *symbol_name;

    for (i = 0; i < memory_address; i++) {
        word = memory[i];

        /* Only print if the memory location is non-zero */
        if (word != 0) {
            printf("Address: %d, ", i + 100);

            /* Check if the word represents a symbol (string) */
            if ((word & 3) == ARE_RELOCATABLE) {
                int string_index = word >> 2;
                if (string_index < string_count) {
                    symbol_name = string_table[string_index];
                    printf("Symbol: %s\n", symbol_name);
                } else {
                    printf("Encoded Word: %015o\n", word);
                }
            } else {
                /* If it's not a symbol, print the encoded word in octal */
                printf("Encoded Word: %015o\n", word);
            }
        }
    }
}