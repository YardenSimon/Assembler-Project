/* macros.c */

#include "macros.h"
#include <string.h>
#include <ctype.h>

/* Array to store macros */
Macro macros[MAX_MACROS];
int macro_count = 0;

/* Known words (used for macro name validation) */
extern const char *group1[];
extern const int group1_count;
extern const char *group2[];
extern const int group2_count;
extern const char *group3[];
extern const int group3_count;
extern const char *instruction_words[];
extern const int instruction_words_count;

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
    char line[256];
    Macro macro;
    strncpy(macro.name, macro_name, sizeof(macro.name));
    macro.name[sizeof(macro.name) - 1] = '\0';
    macro.content[0] = '\0'; /* Initialize content to empty */

    while (fgets(line, sizeof(line), file)) {
        /* Remove trailing newline and carriage return characters */
        line[strcspn(line, "\r\n")] = 0;

        /* Check for the end of macro definition */
        if (strcmp(line, "endmacro") == 0) {
            /* Save the macro */
            if (macro_count < MAX_MACROS) {
                macros[macro_count++] = macro;
            }
            return;
        }
        /* Append line to the macro content */
        strncat(macro.content, line, sizeof(macro.content) - strlen(macro.content) - 1);
        strncat(macro.content, "\n", sizeof(macro.content) - strlen(macro.content) - 1);
    }
}

void replace_macros(const char *input_name, const char *output_name) {
    FILE *input_file = fopen(input_name, "r");
    FILE *output_file = fopen(output_name, "w");
    char line[256];
    int in_macro_definition = 0;
    char macro_name[256] = {0};
    char macro_content[1000] = {0};  /* Temporary storage for macro content*/
    char first_word[256];
    int is_macro = 0;
    int i = 0;

    if (input_file == NULL || output_file == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), input_file)) {
        line[strcspn(line, "\n")] = 0; /* Remove newline*/

        sscanf(line, "%s", first_word);

        if (strcmp(first_word, "macr") == 0) {
            in_macro_definition = 1;
            sscanf(line, "%*s %s", macro_name);
            macro_content[0] = '\0';  /* Reset macro content*/
            continue;
        }

        if (strcmp(line, "endmacr") == 0) {
            if (in_macro_definition) {
                /* Add the macro to the macros array*/
                if (macro_count < MAX_MACROS) {
                    strncpy(macros[macro_count].name, macro_name, sizeof(macros[macro_count].name) - 1);
                    strncpy(macros[macro_count].content, macro_content, sizeof(macros[macro_count].content) - 1);
                    macro_count++;
                }
            }
            in_macro_definition = 0;
            macro_name[0] = '\0';
            continue;
        }

        if (in_macro_definition) {
            strncat(macro_content, line, sizeof(macro_content) - strlen(macro_content) - 1);
            strncat(macro_content, "\n", sizeof(macro_content) - strlen(macro_content) - 1);
        } else {
            /* Check if line starts with a macro name and replace if necessary*/

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