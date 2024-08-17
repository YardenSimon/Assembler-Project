/* first_pass.c */

#include "utils.h"
#include "first_pass.h"
#include "encoder.h"
#include "symbol_table.h"
#include "operand_validation.h"
#include "globals.h"
#include "entry_extern.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int IC = INITIAL_MEMORY_ADDRESS; /* Instruction Counter */
int DC = 0; /* Data Counter */
MachineWord *memory = NULL;
int memory_size = 0;

static void process_line(char *line);

static void handle_label(char *line);

static void handle_instruction(char *line);

static void handle_directive(char *line);


const char *DIRECTIVE_NAMES[NUM_DIRECTIVES] = {
    ".data", ".string", ".entry", ".extern"
};

const char *OPCODE_NAMES[NUM_OPCODES] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* This function initiates the first pass of the assembler. It:
  1. Opens the input file containing assembly code
  2. Initializes the memory array to store the program
  3. Processes the file line by line, calling process_line for each
  4. Handles any file-related errors
 */
void perform_first_pass(const char *filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH + 1];

    file = safe_fopen(filename, "r");

    IC = 100;
    DC = 0;

    memory_size = MEMORY_SIZE;
    memory = (MachineWord *) safe_malloc(memory_size * sizeof(MachineWord *));
    memset(memory, 0, memory_size * sizeof(MachineWord *));

    init_symbol_table();
    init_entry_extern();

    while (safe_fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == ';' || line[0] == '\0') continue;
        process_line(line);
        printf("\n");
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
static void process_line(char *line) {
    /* Skip leading whitespace */
    skip_whitespace(&line);

    if (is_label(line)) {
        handle_label(line);
        /* Move past the label for further processing */
        line = strchr(line, ':') + 1;
        skip_whitespace(&line);
    }

    if (strncmp(line, DIRECTIVE_NAMES[0], strlen(DIRECTIVE_NAMES[0])) == 0 ||
        strncmp(line, DIRECTIVE_NAMES[1], strlen(DIRECTIVE_NAMES[1])) == 0) {
        handle_directive(line);
        } else  if (strncmp(line, DIRECTIVE_NAMES[2], strlen(DIRECTIVE_NAMES[2])) == 0) {
            /* .entry directive */
            line += strlen(DIRECTIVE_NAMES[2]);
            skip_whitespace(&line);
            add_entry(line);
        } else if (strncmp(line, DIRECTIVE_NAMES[3], strlen(DIRECTIVE_NAMES[3])) == 0) {
            /* .extern directive */
            line += strlen(DIRECTIVE_NAMES[3]);
            skip_whitespace(&line);
            add_extern(line);
        } else {
            handle_instruction(line);
        }
}

/*
  When a label is encountered, this function:
  1. Extracts the label from the line
  2. Validates the label length
  3. Adds the label to the symbol table with its current address (IC)
 */
static void handle_label(char *line) {
    char label[MAX_LABEL_LENGTH + 1];
    char *colon = strchr(line, ':');
    size_t label_length = colon - line;

    if (label_length > MAX_LABEL_LENGTH) {
        fprintf(stderr, "Error: Label too long\n");
        return;
    }

    strncpy(label, line, label_length);
    label[label_length] = '\0';
    add_symbol(label, IC+DC);
    line = colon + 1;
    skip_whitespace(&line);
}

/* This function processes assembly instructions:
   1. Validates the instruction using find_command
   2. If valid, calls encode_instruction to convert it to machine code
   3. Manages any instruction-related errors
 */
static void handle_instruction(char *line) {
    OpCode command = find_command(line);
    if (command == -1) {
        fprintf(stderr, "Error: Invalid instruction: %s\n", line);
        return;
    }

    /* The actual encoding is done by encode_instruction in encoder.c */
    encode_instruction(line, command);
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
    int words_encoded;
    char value_str[20];  // Buffer to hold string representation of value

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
            /////////////////// encode_directive - data ///////////////////////////
            sprintf(value_str, "%d", value);  // Convert int to string
            encode_directive(".data", value_str);
            ///         next memory cell

        }
    } else if (strncmp(line, ".string", 7) == 0) {
        line += 7;
        skip_whitespace(&line);
        if (*line != '"') {
            fprintf(stderr, "Error: String must start with a quote\n");
            return;
        }
        /////////////////// encode_directive - string ///////////////////////////
        encode_directive(".string", line);

        ///         next memory cell
        while (*line && *line != '"') {
            DC++;
            line++;
        }
        /* Add null terminator */
    }
}

void free_memory(void) {
    if (memory != NULL) {
        free(memory);
        memory = NULL;
    }
    memory_size = 0;
    IC = 100; // Reset Instruction Counter
    DC = 0; // Reset Data Counter
}