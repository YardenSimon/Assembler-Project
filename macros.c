// macros.c

#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Array to store macros
Macro macros[MAX_MACROS];
int macro_count = 0;

// Known words (used for macro name validation)
extern const char *group1[];
extern const int group1_count;
extern const char *group2[];
extern const int group2_count;
extern const char *group3[];
extern const int group3_count;
extern const char *instruction_words[];
extern const int instruction_words_count;

// Function to check if a token is in a given list
int is_in_list(const char *token, const char *list[], int list_count) {
    for (int i = 0; i < list_count; i++) {
        if (strcmp(token, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a token is a valid macro name
int is_valid_macro_name(const char *token) {
    // Check if the token is a known word
    if (is_in_list(token, group1, group1_count) ||
        is_in_list(token, group2, group2_count) ||
        is_in_list(token, group3, group3_count) ||
        is_in_list(token, instruction_words, instruction_words_count)) {
        return 0;
    }
    // Any name that is not a known word is considered valid
    return 1;
}

// Function to process lines within a macro definition
void process_macro_definition(const char *macro_name, FILE *file) {
    char line[256];
    Macro macro;
    strncpy(macro.name, macro_name, sizeof(macro.name));
    macro.name[sizeof(macro.name) - 1] = '\0';
    macro.content[0] = '\0'; // Initialize content to empty

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline and carriage return characters
        line[strcspn(line, "\r\n")] = 0;

        // Check for the end of macro definition
        if (strcmp(line, "endmacr") == 0) {
            // Save the macro
            if (macro_count < MAX_MACROS) {
                macros[macro_count++] = macro;
            } else {
                printf("Error: Macro storage limit reached.\n");
            }
            return;
        }
        // Append line to the macro content
        strncat(macro.content, line, sizeof(macro.content) - strlen(macro.content) - 1);
        strncat(macro.content, "\n", sizeof(macro.content) - strlen(macro.content) - 1);
    }
}
