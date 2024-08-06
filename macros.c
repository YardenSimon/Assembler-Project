/* macros.c */

#include "macros.h"
#include "opcode_groups.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* Dynamic array to store macros */
Macro* macros = NULL;
int macro_count = 0;
int macro_capacity = 0;

/* Known words (used for macro name validation) */
extern const char *group1[];
extern const int group1_count;
extern const char *group2[];
extern const int group2_count;
extern const char *group3[];
extern const int group3_count;
extern const char *instruction_words[];
extern const int instruction_words_count;

void init_macros() {
    macro_capacity = 10;  /* Start with space for 10 macros */
    macros = (Macro*)calloc(macro_capacity, sizeof(Macro));
    if (macros == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for macros\n");
        exit(1);
    }
}

/* Function to check if a word is in a given list */
int word_in_list(const char *word, const char *list[], int list_count) {
    int i;
    for (i = 0; i < list_count; i++) {
        if (strcmp(word, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Function to check if a word can be a macro name */
int can_be_macro_name(const char *word) {
    /* Check if the word is a known word */
    if (word_in_list(word, group1, group1_count) ||
        word_in_list(word, group2, group2_count) ||
        word_in_list(word, group3, group3_count) ||
        word_in_list(word, instruction_words, instruction_words_count)) {
        return 0;
    }
    /* Any name that is not a known word is considered valid */
    return 1;
}

/* Function to handle lines inside a macro */
void handle_macro_inside(const char *macro_name, FILE *file) {
    char line[MAX_MACRO_CONTENT];
    char* content = NULL;
    size_t content_size = 0;
    size_t content_capacity = MAX_MACRO_CONTENT;

    content = (char*)malloc(content_capacity);
    if (content == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for macro content\n");
        exit(1);
    }
    content[0] = '\0';

    while (fgets(line, sizeof(line), file)) {
        /* Remove trailing newline and carriage return characters */
        line[strcspn(line, "\r\n")] = 0;

        /* Check for the end of macro definition */
        if (strcmp(line, "endmacro") == 0) {
            /* Save the macro */
            if (macro_count >= macro_capacity) {
                macro_capacity *= 2;
                Macro* temp = (Macro*)realloc(macros, macro_capacity * sizeof(Macro));
                if (temp == NULL) {
                    fprintf(stderr, "Error: Memory reallocation failed for macros\n");
                    free(content);
                    exit(1);
                }
                macros = temp;
            }
            strncpy(macros[macro_count].name, macro_name, MAX_MACRO_NAME - 1);
            macros[macro_count].name[MAX_MACRO_NAME - 1] = '\0';
            macros[macro_count].content = content;
            macro_count++;
            return;
        }
        /* Append line to the macro content */
        content_size += strlen(line) + 1; /* +1 for newline */
        if (content_size >= content_capacity) {
            content_capacity *= 2;
            char* temp = (char*)realloc(content, content_capacity);
            if (temp == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed for macro content\n");
                free(content);
                exit(1);
            }
            content = temp;
        }
        strcat(content, line);
        strcat(content, "\n");
    }
    free(content); /* Free if endmacro not found */
}

void replace_macros(const char *input_name, const char *output_name) {
    FILE *input_file = fopen(input_name, "r");
    FILE *output_file = fopen(output_name, "w");
    char line[MAX_MACRO_CONTENT];
    int in_macro_definition = 0;
    char macro_name[MAX_MACRO_NAME] = {0};
    char* macro_content = NULL;
    char first_word[MAX_MACRO_NAME];
    int is_macro = 0;
    int i = 0;

    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Error opening files\n");
        return;
    }

    while (fgets(line, sizeof(line), input_file)) {
        line[strcspn(line, "\n")] = 0; /* Remove newline */

        sscanf(line, "%s", first_word);

        if (strcmp(first_word, "macro") == 0) {
            in_macro_definition = 1;
            sscanf(line, "%*s %s", macro_name);
            if (macro_content != NULL) {
                free(macro_content);
            }
            macro_content = (char*)malloc(MAX_MACRO_CONTENT);
            if (macro_content == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for macro content\n");
                fclose(input_file);
                fclose(output_file);
                return;
            }
            macro_content[0] = '\0';
            continue;
        }

        if (strcmp(line, "endmacro") == 0) {
            if (in_macro_definition) {
                /* Add the macro to the macros array */
                if (macro_count >= macro_capacity) {
                    macro_capacity *= 2;
                    Macro* temp = (Macro*)realloc(macros, macro_capacity * sizeof(Macro));
                    if (temp == NULL) {
                        fprintf(stderr, "Error: Memory reallocation failed for macros\n");
                        free(macro_content);
                        fclose(input_file);
                        fclose(output_file);
                        return;
                    }
                    macros = temp;
                }
                strncpy(macros[macro_count].name, macro_name, MAX_MACRO_NAME - 1);
                macros[macro_count].name[MAX_MACRO_NAME - 1] = '\0';
                macros[macro_count].content = macro_content;
                macro_count++;
            }
            in_macro_definition = 0;
            macro_name[0] = '\0';
            macro_content = NULL;
            continue;
        }

        if (in_macro_definition) {
            strcat(macro_content, line);
            strcat(macro_content, "\n");
        } else {
            /* Check if line starts with a macro name and replace if necessary */
            is_macro = 0;
            for (i = 0; i < macro_count; i++) {
                if (strcmp(first_word, macros[i].name) == 0) {
                    fputs(macros[i].content, output_file);
                    is_macro = 1;
                    break;
                }
            }
            if (!is_macro) {
                fputs(line, output_file);
                fputc('\n', output_file);
            }
        }
    }

    fclose(input_file);
    fclose(output_file);
}

void free_macros() {
    int i;
    for (i = 0; i < macro_count; i++) {
        free(macros[i].content);
    }
    free(macros);
    macros = NULL;
    macro_count = 0;
    macro_capacity = 0;
}