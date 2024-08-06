/* first_pass.c */

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
MachineWord** memory = NULL;
int memory_size = 0;

static void process_line(char* line);
static void handle_label(char* line);
static void handle_instruction(char* line);
static void handle_directive(char* line);
static int add_to_memory(MachineWord* word);
static void free_memory_content(void);

/* This function initiates the first pass of the assembler. It:
  1. Opens the input file containing assembly code
  2. Initializes the memory array to store the program
  3. Processes the file line by line, calling process_line for each
  4. Handles any file-related errors
 */
void perform_first_pass(const char* filename) {
    FILE* file;
    char line[MAX_LINE_LENGTH + 1];

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    IC = 100;
    DC = 0;

    memory_size = INITIAL_MEMORY_SIZE;
    memory = (MachineWord**)calloc(memory_size, sizeof(MachineWord*));
    if (memory == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return;
    }

    init_symbol_table();

    while (fgets(line, sizeof(line), file)) {
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
    if (is_label(line)) {
        handle_label(line);
        /* Move past the label for further processing */
        line = strchr(line, ':') + 1;
        while (isspace((unsigned char)*line)) line++;
    }

    if (strncmp(line, ".data", 5) == 0 || strncmp(line, ".string", 7) == 0) {
        handle_directive(line);
    } else if (strncmp(line, ".extern", 7) == 0) {
        /* Handle .extern directive */
        line += 7;
        while (isspace((unsigned char)*line)) line++;
        add_symbol(line, 0);
    } else if (strncmp(line, ".entry", 6) == 0) {
        /* For .entry, mark the symbol as entry (in second pass) */
        /* This will be handled in the second pass */
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
    while (isspace((unsigned char)*line)) line++;
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
    char* token;
    int value;
    MachineWord* word;

    if (strncmp(line, ".data", 5) == 0) {
        line += 5;
        while (*line) {
            while (isspace((unsigned char)*line) || *line == ',') line++;
            if (*line == '\0') break;

            value = (int)strtol(line, &line, 10);
            word = (MachineWord*)malloc(sizeof(MachineWord));
            if (word == NULL) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                return;
            }
            *word = (MachineWord)value;
            if (add_to_memory(word) == -1) {
                free(word);
                return;
            }
            DC++;
        }
    } else if (strncmp(line, ".string", 7) == 0) {
        line += 7;
        while (isspace((unsigned char)*line)) line++;
        if (*line != '"') {
            fprintf(stderr, "Error: String must start with a quote\n");
            return;
        }
        line++;
        while (*line && *line != '"') {
            word = (MachineWord*)malloc(sizeof(MachineWord));
            if (word == NULL) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                return;
            }
            *word = (MachineWord)(unsigned char)*line;
            if (add_to_memory(word) == -1) {
                free(word);
                return;
            }
            DC++;
            line++;
        }
        /* Add null terminator */
        word = (MachineWord*)malloc(sizeof(MachineWord));
        if (word == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }
        *word = 0;
        if (add_to_memory(word) == -1) {
            free(word);
            return;
        }
        DC++;
    }
}
/* This function manages memory allocation:
   1. Calculates the current memory address
   2. Expands memory if necessary using realloc
   3. Adds the new word to the memory array
   4. Handles memory allocation failures
 */
static int add_to_memory(MachineWord* word) {
    int current_address = IC + DC - 100;
    if (current_address >= memory_size) {
        MachineWord** new_memory;
        memory_size *= 2;
        new_memory = (MachineWord**)realloc(memory, memory_size * sizeof(MachineWord*));
        if (new_memory == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed\n");
            return -1;
        }
        memory = new_memory;
    }
    memory[current_address] = word;
    return 0;
}

void free_memory(void) {
    free_memory_content();
    free(memory);
    memory = NULL;
    memory_size = 0;
    free_symbol_table();
}

static void free_memory_content(void) {
    int i;
    for (i = 0; i < IC + DC - 100; i++) {
        if (memory[i] != NULL) {
            free(memory[i]);
        }
    }
}