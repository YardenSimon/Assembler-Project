/* first_pass.c */

#include "utils.h"
#include "first_pass.h"
#include "encoder.h"
#include "symbol_table.h"
#include "operand_validation.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define INITIAL_MEMORY_SIZE 1000

int IC = 100; /* Instruction Counter */
int DC = 0;   /* Data Counter */
MachineWord* memory = NULL;
int memory_size = 0;

static void process_line(char* line);
static void handle_label(char* line);
static void handle_instruction(char* line);
static void handle_directive(char* line);
static int add_to_memory(MachineWord word);

/* This function initiates the first pass of the assembler. It:
  1. Opens the input file containing assembly code
  2. Initializes the memory array to store the program
  3. Processes the file line by line, calling process_line for each
  4. Handles any file-related errors
 */
void perform_first_pass(const char* filename) {
    FILE* file;
    char line[MAX_LINE_LENGTH + 1];

    file = safe_fopen(filename, "r");

    IC = 100;
    DC = 0;

    memory_size = INITIAL_MEMORY_SIZE;
    memory = (MachineWord*)safe_malloc(memory_size * sizeof(MachineWord*));
    memset(memory, 0, memory_size * sizeof(MachineWord*));

    init_symbol_table();

    while (safe_fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == ';' || line[0] == '\0') continue;
        process_line(line);
    }

    fclose(file);
}

/*
 This function analyzes each line of the input:
 1. Identifies and processes labels using handle_label
 2. Determines the line type (directive or instruction)
 3. Calls appropriate handlers (handle_directive or handle_instruction)
 4. Manages .extern directives by adding them to the symbol table
 */
static void process_line(char* line) {

    /* Skip leading whitespace */
    skip_whitespace(&line);

    if (is_label(line)) {
        handle_label(line);
        /* Move past the label for further processing */
        line = strchr(line, ':') + 1;
        skip_whitespace(&line);
    }

    if (strncmp(line, ".data", 5) == 0 || strncmp(line, ".string", 7) == 0) {
        handle_directive(line);
    } else if (strncmp(line, ".extern", 7) == 0) {
        /* Handle .extern directive */
        line += 7;
        skip_whitespace(&line);
        add_symbol(line, IC+DC);
        IC++;
        symbol_table[symbol_count-1].is_external = 1;
    } else if (strncmp(line, ".entry", 6) == 0) {
        /* For .entry, mark the symbol as entry (in second pass) */
        /* This will be handled in the second pass */
        line += 7;
        skip_whitespace(&line);
        add_symbol(line, IC+DC);
        IC++;
        symbol_table[symbol_count-1].is_entry = 1;
    } else if (line[0] == '.') {
        /* Handle unknown directives */
        fprintf(stderr, "Error: Unknown directive: %s\n", line);
    } else if (line[0] != '\0') {
        /* Only process non-empty lines as instructions */
        handle_instruction(line);
    }
}

/*
  When a label is encountered, this function:
  1. Extracts the label from the line
  2. Validates the label length
  3. Adds the label to the symbol table with its current address (IC)
 */
static void handle_label(char* line) {
    char label[MAX_LABEL_LENGTH + 1];
    char* colon = strchr(line, ':');
    size_t label_length = colon - line;

    if (label_length > MAX_LABEL_LENGTH) {
        fprintf(stderr, "Error: Label too long\n");
        return;
    }

    strncpy(label, line, label_length);
    label[label_length] = '\0';
    add_symbol(label, IC);
    line = colon + 1;
    skip_whitespace(&line);
}

/* This function processes assembly instructions:
   1. Validates the instruction using find_command
   2. If valid, calls encode_instruction to convert it to machine code
   3. Manages any instruction-related errors
 */
static void handle_instruction(char* line) {
    int command_index = find_command(line);
    if (command_index == -1) {
        fprintf(stderr, "Error: Invalid instruction: %s\n", line);
        return;
    }

    /* The actual encoding is done by encode_instruction in encoder.c */
    encode_instruction(line);
    /* IC is incremented in encode_instruction */
}

/* This function processes assembler directives (.data and .string):
   1. For .data: Parses numbers, allocates memory for each, and stores them
   2. For .string: Allocates memory for each character and its ASCII value
   3. Adds a null terminator for strings
   4. Updates the Data Counter (DC) accordingly
 */
static void handle_directive(char* line) {
    int value;
    char* endptr;

    if (strncmp(line, ".data", 5) == 0) {
        line += 5;
        while (*line) {
            while (isspace((unsigned char)*line) || *line == ',') line++;
            if (*line == '\0') break;

            value = (int)strtol(line, &endptr, 10);
            if (line == endptr) {
                fprintf(stderr, "Error: Invalid number in .data directive\n");
                return;
            }
            line = endptr;

            DC++;
        }
    } else if (strncmp(line, ".string", 7) == 0) {
        line += 7;
        skip_whitespace(&line);
        if (*line != '"') {
            fprintf(stderr, "Error: String must start with a quote\n");
            return;
        }
        line++;
        while (*line && *line != '"') {
            DC++;
            line++;
        }
        /* Add null terminator */
        DC++;
    }
}

/* This function manages memory allocation:
   1. Calculates the current memory address
   2. Expands memory if necessary using realloc
   3. Adds the new word to the memory array
   4. Handles memory allocation failures
 */
static int add_to_memory(MachineWord word) {
    int current_address = IC + DC - 100;
    if (current_address >= memory_size) {
        MachineWord* new_memory;
        memory_size *= 2;
        new_memory = (MachineWord*)safe_malloc(memory_size * sizeof(MachineWord*));
        memset(memory, 0, memory_size * sizeof(MachineWord*));
        memory = new_memory;
    }
    memory[current_address] = word;
    return 0;
}

void free_memory(void) {
    if (memory != NULL) {
        free(memory);
        memory = NULL;
    }
    memory_size = 0;
    IC = 100;  // Reset Instruction Counter
    DC = 0;    // Reset Data Counter
}

