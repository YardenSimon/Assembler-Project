#include "utils.h"
#include "first_pass.h"
#include "encoder.h"
#include "symbol_table.h"
#include "operand_validation.h"
#include "globals.h"
#include "entry_extern.h"
#include "errors.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int IC = INITIAL_MEMORY_ADDRESS;
int DC = 0;
MachineWord *memory = NULL;
int memory_size = 0;
int current_line_number = 0;
DataSection data_sections[MAX_DATA_SECTIONS];
int data_section_count = 0;

const char *DIRECTIVE_NAMES[NUM_DIRECTIVES] = {
    ".data", ".string", ".entry", ".extern"
};

const char *OPCODE_NAMES[NUM_OPCODES] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/*
 * Processes a single line of assembly code.
 * This function determines if the line contains a label, directive, or instruction and handles it accordingly.
 */
static void process_line(char *line, int line_number);

/*
 * Handles a label in the assembly code.
 * This function checks if the label is valid, adds it to the symbol table, and ensures it doesn't conflict with reserved words or macros.
 * It returns 1 if the label is valid and added successfully, 0 otherwise.
 */
static int handle_label(char *line, int line_number);

/*
 * Handles an instruction in the assembly code.
 * This function identifies the instruction and encodes it into machine code.
 * If the instruction is invalid, an error is logged.
 */
static void handle_instruction(char *line, int line_number);

/*
 * This function deals with special instructions in the assembly code, like storing numbers or text.
 * It saves these values in memory and keeps track of where this data is stored.
 */
static void handle_directive(char *line, int line_number);


void add_data_section(int start_address, int end_address) {
    if (data_section_count < MAX_DATA_SECTIONS) {
        data_sections[data_section_count].start_address = start_address;
        data_sections[data_section_count].end_address = end_address;
        data_section_count++;
    } else {
        add_error(ERROR_MEMORY_ALLOCATION, current_filename, current_line_number,
                  "Too many data sections. Maximum allowed: %d", MAX_DATA_SECTIONS);
    }
}

void perform_first_pass(const char *filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH + 1];
    current_line_number = 0;
    file = safe_fopen(filename, "r");

    IC = INITIAL_MEMORY_ADDRESS;
    DC = 0;

    memory_size = MEMORY_SIZE;
    memory = (MachineWord *) safe_malloc(memory_size * sizeof(MachineWord *));
    if (memory == NULL) {
        add_error(ERROR_MEMORY_ALLOCATION, filename, current_line_number, "Failed to allocate memory");
        return;
    }
    memset(memory, 0, memory_size * sizeof(MachineWord *));

    init_symbol_table();
    init_entry_extern();

    while (safe_fgets(line, sizeof(line), file) != NULL) {
        current_line_number++;
        line[strcspn(line, "\n")] = 0;
        if (line[0] == ';' || line[0] == '\0') continue;
        process_line(line, current_line_number);
        printf("\n");
    }

    fclose(file);
}

static void process_line(char *line, int line_number) {
    char *end;

    skip_whitespace(&line);
    if (is_label(line)) {
        if (!handle_label(line, line_number)) {
            return;
        }
        line = strchr(line, ':') + 1;
        skip_whitespace(&line);
    }

    /* Check if it's a directive line */
    if (strncmp(line, DIRECTIVE_NAMES[0], strlen(DIRECTIVE_NAMES[0])) == 0 ||
        strncmp(line, DIRECTIVE_NAMES[1], strlen(DIRECTIVE_NAMES[1])) == 0) {
        handle_directive(line, line_number);
    } else if (strncmp(line, DIRECTIVE_NAMES[2], strlen(DIRECTIVE_NAMES[2])) == 0) {
        /* .entry directive */
        line += strlen(DIRECTIVE_NAMES[2]);
        skip_whitespace(&line);
        end = line + strlen(line) - 1;
        while (end > line && isspace((unsigned char) *end)) {
            *end = '\0';
            end--;
        }
        add_entry(line);
    } else if (strncmp(line, DIRECTIVE_NAMES[3], strlen(DIRECTIVE_NAMES[3])) == 0) {
        /* .extern directive */
        line += strlen(DIRECTIVE_NAMES[3]);
        skip_whitespace(&line);
        end = line + strlen(line) - 1;
        while (end > line && isspace((unsigned char) *end)) {
            *end = '\0';
            end--;
        }
        if (!is_valid_label_name(line)) {
            add_error(ERROR_INVALID_LABEL, current_filename, line_number, "Invalid extern label name: %s", line);
        } else if (is_label_name(line)) {
            add_error(ERROR_SYMBOL_CONFLICT, current_filename, line_number,
                      "Symbol already defined as a label: %s", line);
        } else if (is_macro(line)) {
            add_error(ERROR_SYMBOL_CONFLICT, current_filename, line_number,
                      "Symbol already defined as a macro: %s", line);
        } else {
            add_extern(line);
        }
    } else {
        handle_instruction(line, line_number);
    }
}

static int handle_label(char *line, int line_number) {
    char label[MAX_LABEL_LENGTH + 1];
    char *colon = strchr(line, ':');
    size_t label_length = colon - line;

    if (label_length > MAX_LABEL_LENGTH) {
        add_error(ERROR_INVALID_LABEL, current_filename, line_number, "Label too long: %.*s",
                  (int) label_length, line);
        return 0;
    }

    strncpy(label, line, label_length);
    label[label_length] = '\0';

    if (!is_valid_label_name(label)) {
        add_error(ERROR_INVALID_LABEL, current_filename, line_number, "Invalid label name: %s", label);
        return 0;
    }

    if (is_reserved_word(label)) {
        add_error(ERROR_RESERVED_WORD_AS_LABEL, current_filename, line_number,
                  "Reserved word used as label: %s", label);
        return 0;
    }

    if (is_label_name(label)) {
        add_error(ERROR_DUPLICATE_LABEL_DEFINITION, current_filename, line_number,
                  "Duplicate label definition: %s", label);
        return 0;
    }

    if (is_macro(label)) {
        add_error(ERROR_SYMBOL_CONFLICT, current_filename, line_number,
                  "Label name conflicts with macro name: %s", label);
        return 0;
    }

    add_symbol(label, memory_address);
    return 1;
}

static void handle_instruction(char *line, int line_number) {
    OpCode command = find_command(line);

    if (command == -1) {
        add_error(ERROR_INVALID_INSTRUCTION, current_filename, line_number, "Invalid instruction: %s", line);
        return;
    }
    encode_instruction(line, command);
}

static void handle_directive(char *line, int line_number) {
    int value;
    char *endptr;
    char value_str[20];
    int start_address, end_address;

    if (strncmp(line, ".data", 5) == 0) {
        start_address = memory_address;
        line += 5;
        while (*line) {
            while (isspace((unsigned char) *line) || *line == ',') line++;
            if (*line == '\0') break;

            value = (int) strtol(line, &endptr, 10);
            if (line == endptr) {
                add_error(ERROR_INVALID_OPERAND, current_filename, line_number, "Invalid number in .data directive");
                return;
            }
            line = endptr;
            sprintf(value_str, "%d", value);
            encode_directive(".data", value_str);
        }
        end_address = memory_address - 1;
        add_data_section(start_address, end_address);
    } else if (strncmp(line, ".string", 7) == 0) {
        start_address = memory_address;
        line += 7;
        skip_whitespace(&line);
        if (*line != '"') {
            add_error(ERROR_INVALID_OPERAND, current_filename, line_number, "String must start with a quote");
            return;
        }
        encode_directive(".string", line);

        while (*line && *line != '"') {
            DC++;
            line++;
        }
        end_address = memory_address - 1;
        add_data_section(start_address, end_address);
    }
}

void free_memory(void) {
    if (memory != NULL) {
        free(memory);
        memory = NULL;
    }
    memory_size = 0;
    IC = 100;
    DC = 0;
}
