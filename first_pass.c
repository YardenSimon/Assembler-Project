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

int IC = INITIAL_MEMORY_ADDRESS;
int DC = 0;
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

/*
 * This function initiates the first pass of the assembler. It:
 * Opens the input file containing assembly code.
 * Initializes the memory array to store the program.
 * Processes the file line by line, calling process_line function for each line.
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
 * This function analyzes each line of the input and:
 * Identifies and processes labels using handle_label.
 * Determines the type of the line (directive or instruction).
 * Calls appropriate handlers (handle_directive or handle_instruction).
 * Manages .extern directives by adding them to the symbol table.
 */
static void process_line(char *line) {
    skip_whitespace(&line);

    if (is_label(line)) {
        handle_label(line);
        line = strchr(line, ':') + 1;
        skip_whitespace(&line);
    }

    /* Check if it's a directive line */
    if (strncmp(line, DIRECTIVE_NAMES[0], strlen(DIRECTIVE_NAMES[0])) == 0 ||
        strncmp(line, DIRECTIVE_NAMES[1], strlen(DIRECTIVE_NAMES[1])) == 0) {
        handle_directive(line);
    } else if (strncmp(line, DIRECTIVE_NAMES[2], strlen(DIRECTIVE_NAMES[2])) == 0) {
        /* .entry directive */
        line += strlen(DIRECTIVE_NAMES[2]);
        skip_whitespace(&line);
        add_entry(*line);
    } else if (strncmp(line, DIRECTIVE_NAMES[3], strlen(DIRECTIVE_NAMES[3])) == 0) {
        /* .extern directive */
        line += strlen(DIRECTIVE_NAMES[3]);
        skip_whitespace(&line);
        add_extern(*line, -1);
    } else {
        handle_instruction(line);
    }
}

/*
 * When a label is encountered, this function:
 * Extracts the label from the line.
 * Validates the label length to ensure it doesn't exceed the maximum allowed.
 * Adds the label to the symbol table with its current address (IC + DC).
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
    add_symbol(label, IC + DC);

    /* Move the line pointer after the label */
    line = colon + 1;
    skip_whitespace(&line);
}

/*
 * This function processes assembly instructions:
 * Validates the instruction using find_command to ensure it's a recognized opcode.
 * If valid, calls encode_instruction to convert the instruction to machine code..
 */
static void handle_instruction(char *line) {
    OpCode command = find_command(line);

    /* Check if the instruction is valid */
    if (command == -1) {
        fprintf(stderr, "Error: Invalid instruction: %s\n", line);
        return;
    }

    encode_instruction(line, command);
}

/*
 * This function processes assembler directives such as .data and .string:
 * For .data: Parses numbers from the line, allocates memory for each, and stores them in the data section.
 * For .string: Allocates memory for each character in the string, storing their ASCII values.
 * Adds a null terminator for strings to mark the end of the string.
 * Updates the Data Counter (DC) accordingly.
 */
static void handle_directive(char *line) {
    int value;
    char *endptr;
    /* int words_encoded; - UNUSED - CHECK!!!!!!!!!!!!!!!*/
    /* Buffer to hold string representation of value */
    char value_str[20];


    /* .data directive */
    if (strncmp(line, ".data", 5) == 0) {
        line += 5;
        while (*line) {
            while (isspace((unsigned char) *line) || *line == ',') line++;
            if (*line == '\0') break;

            value = (int) strtol(line, &endptr, 10);
            if (line == endptr) {
                fprintf(stderr, "Error: Invalid number in .data directive\n");
                return;
            }
            line = endptr;
            sprintf(value_str, "%d", value);
            encode_directive(".data", value_str);
        }
    } else if (strncmp(line, ".string", 7) == 0) {
        /* .string directive */
        line += 7;
        skip_whitespace(&line);
        if (*line != '"') {
            fprintf(stderr, "Error: String must start with a quote\n");
            return;
        }
        encode_directive(".string", line);

        while (*line && *line != '"') {
            DC++;
            line++;
        }
    }
}

/*
 * This function frees the memory allocated during the first pass.
 * It resets the memory pointer and counters (IC and DC) to their initial values.
 */
void free_memory(void) {
    if (memory != NULL) {
        free(memory);
        memory = NULL;
    }
    memory_size = 0;
    IC = 100;
    DC = 0;
}
