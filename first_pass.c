#include "first_pass.h"
#include "encoder.h"
#include <string.h>
#include <ctype.h>

void add_symbol(const char* name, int address) {
    /* Add a symbol to the symbol table if there is space */
    if (symbol_count < MAX_SYMBOLS) {
        strncpy(symbol_table[symbol_count].name, name, 30);
        symbol_table[symbol_count].name[30] = '\0';
        symbol_table[symbol_count].address = address;
        symbol_count++;
    } else {
        printf("Error: Symbol table full\n");
    }
}

int is_label(const char* line) {
    const char* colon;
    /* Check if the line contains a label */
    colon = strchr(line, ':');
    return colon != NULL && colon != line;
}

void get_label_name(const char* line, char* label, int max_length) {
    int i;
    /* Extract the label name from the line */
    for (i = 0; i < max_length - 1 && line[i] != ':' && line[i] != '\0'; i++) {
        label[i] = line[i];
    }
    label[i] = '\0';
}

void process_data_directive(const char* directive) {
    /* Handle .data and .string directives */
    /* This function should be implemented to handle data directives */
}

void report_error(const char* message, int line_number) {
    /* Print error message with line number */
    fprintf(stderr, "Error on line %d: %s\n", line_number, message);
}

void first_pass(const char* filename) {
    FILE* file;
    char line[MAX_LINE_LENGTH];
    char label[31];
    char* instruction;
    int line_number;

    /* Open the file for reading */
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    IC = STARTING_ADDRESS;
    DC = 0;
    line_number = 0;

    /* Process each line in the file */
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\n")] = 0; /* Remove newline character */

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == ';') {
            continue;
        }

        if (is_label(line)) {
            instruction = strchr(line, ':');
            if (instruction) {
                *instruction = '\0'; /* Null-terminate the label */
                get_label_name(line, label, sizeof(label));
                add_symbol(label, IC);
                instruction++; /* Move past the colon */
                while (isspace(*instruction)) instruction++;
            }
        } else {
            instruction = line;
        }

        /* Handle different types of instructions */
        if (strncmp(instruction, ".data", 5) == 0 || strncmp(instruction, ".string", 7) == 0) {
            process_data_directive(instruction);
        } else if (instruction[0] != '\0') {
            encode_instruction(instruction);
        }
    }

    DC += IC - STARTING_ADDRESS;

    /* Close the file */
    fclose(file);
}