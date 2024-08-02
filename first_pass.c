/* first_pass.c */

#include "first_pass.h"
#include "encoder.h"
#include "symbol_table.h"
#include "operand_validation.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int IC = 100; /* Instruction Counter */
int DC = 0;   /* Data Counter */
MachineWord* memory = NULL;
int memory_size = 0;

static void process_line(char* line);
static void handle_label(char* line);
static void handle_instruction(char* line);
static void handle_directive(char* line);

void perform_first_pass(const char* filename) {
    FILE* file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH + 1];

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    IC = 100; /* Starting address */
    DC = 0;

    /* Initialize memory */
    memory_size = 1000; /* Start with space for 1000 words */
    memory = (MachineWord*)calloc(memory_size, sizeof(MachineWord));
    if (memory == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return;
    }

    init_symbol_table();

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; /* Remove newline */
        if (line[0] == ';' || line[0] == '\0') continue; /* Skip comments and empty lines */
        process_line(line);
    }

    fclose(file);
}

static void process_line(char* line) {
    if (is_label(line)) {
        handle_label(line);
    }
    while (isspace(*line)) line++;
    if (strncmp(line, ".data", 5) == 0 || strncmp(line, ".string", 7) == 0) {
        handle_directive(line);
    } else if (strncmp(line, ".extern", 7) == 0 || strncmp(line, ".entry", 6) == 0) {
        /* Handle .extern and .entry directives */
        /* For .extern, add to symbol table with address 0 */
        /* For .entry, mark the symbol as entry (in second pass) */
    } else {
        handle_instruction(line);
    }
}

static void handle_label(char* line) {
    char label[MAX_LINE_LENGTH];
    char* colon = strchr(line, ':');
    strncpy(label, line, colon - line);
    label[colon - line] = '\0';
    add_symbol(label, IC);
    line = colon + 1;
    while (isspace(*line)) line++;
}

static void handle_instruction(char* line) {
    char first_operand[MAX_LINE_LENGTH], second_operand[MAX_LINE_LENGTH];
    extract_operands(line, first_operand, second_operand);
    if (validate_operand(first_operand, 0) && validate_operand(second_operand, 0)) {
        encode_instruction(line);
    } else {
        fprintf(stderr, "Invalid operands in line: %s\n", line);
    }
}

static void handle_directive(char* line) {
    if (strncmp(line, ".data", 5) == 0) {
        /* Handle .data directive */
        /* Parse numbers and add to data section */
        /* Increment DC for each number */
    } else if (strncmp(line, ".string", 7) == 0) {
        /* Handle .string directive */
        /* Parse string and add each character to data section */
        /* Increment DC for each character plus null terminator */
    }
}

void free_memory() {
    free(memory);
    memory = NULL;
    memory_size = 0;
    free_symbol_table();
}