// input_check.c

#include <stdio.h>
#include <string.h>
#include "macros.h"  // Include the header file for macros handling

// Define known mnemonics and instruction words
const char *group1[] = {"mov", "cmp", "add", "sub", "lea"};
const int group1_count = 5;

const char *group2[] = {"clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
const int group2_count = 9;

const char *group3[] = {"rts", "stop"};
const int group3_count = 2;

const char *instruction_words[] = {".data", ".string", ".entry", ".extern"};
const int instruction_words_count = 4;

// Function to check if a token is in a given list
int is_in_list(const char *token, const char *list[], int list_count) {
    for (int i = 0; i < list_count; i++) {
        if (strcmp(token, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a token is a known word
int is_known_word(const char *token) {
    return is_in_list(token, group1, group1_count) ||
           is_in_list(token, group2, group2_count) ||
           is_in_list(token, group3, group3_count) ||
           is_in_list(token, instruction_words, instruction_words_count);
}

// Function to process and validate each line of the input file
void process_line(const char *line, FILE *file) {
    char buffer[256];
    strncpy(buffer, line, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

    // Ignore comment lines
    if (line[0] == ';') {
        return;
    }

    // Tokenize the line to get the first word
    char *token = strtok(buffer, " \t\n");

    // Handle macro definitions
    if (token != NULL && strcmp(token, "macr") == 0) {
        token = strtok(NULL, " \t\n");
        if (token != NULL && is_valid_macro_name(token)) {
            // Check if there are more tokens, which would be invalid
            if (strtok(NULL, " \t\n") != NULL) {
                printf("Error: Extra tokens after macro name: %s\n", line);
            } else {
                process_macro_definition(token, file);
            }
        } else {
            printf("Error: Invalid macro name: %s\n", token);
        }
        return;
    }

    // Check if the first word is a known word
    if (token != NULL) {
        if (is_known_word(token)) {
            // Valid known word processing (without operands)
        } else {
            printf("Error: Unknown word: %s\n", token);
        }
    }
}

// Function to read and validate the input file
void validate_assembly_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Process each line individually
        process_line(line, file);
    }

    fclose(file);
}
