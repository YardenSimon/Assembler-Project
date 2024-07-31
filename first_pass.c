#include "first_pass.h"
#include "encoder.h"
#include "symbol_table.h"
#include "operand_validation.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int IC;
extern int DC;

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

    if (strncmp(line, ".data", 5) == 0 || strncmp(line, ".string", 7) == 0 ||
        strncmp(line, ".extern", 7) == 0 || strncmp(line, ".entry", 6) == 0) {
        handle_directive(line);
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
    } else if (strncmp(line, ".string", 7) == 0) {
        /* Handle .string directive */
    } else if (strncmp(line, ".extern", 7) == 0) {
        /* Handle .extern directive */
    } else if (strncmp(line, ".entry", 6) == 0) {
        /* Handle .entry directive */
    }
}