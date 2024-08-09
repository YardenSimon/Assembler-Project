#include "macros.h"
#include "opcode_groups.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 256
#define INITIAL_MACRO_CAPACITY 10

Macro* macros = NULL;
int macro_count = 0;
int macro_capacity = 0;

const char *group1[] = {"mov", "cmp", "add", "sub", "lea"};
const int group1_count = sizeof(group1) / sizeof(group1[0]);

const char *group2[] = {"clr", "not", "inc", "dec", "jmp", "bne", "jsr"};
const int group2_count = sizeof(group2) / sizeof(group2[0]);

const char *group3[] = {"red", "prn", "rts", "stop"};
const int group3_count = sizeof(group3) / sizeof(group3[0]);

const char *instruction_words[] = {".data", ".string", ".entry", ".extern"};
const int instruction_words_count = sizeof(instruction_words) / sizeof(instruction_words[0]);

static int word_in_list(const char *word, const char *list[], int list_count);
static char* read_line(FILE *file);
static void resize_macro_array(void);
static Macro* find_macro(const char *name);
static void process_macros(const char *input_name, const char *output_name);
static int handle_macro_start(char *line, char *macro_name, int line_number);
static void handle_macro_end(char *macro_name, char **macro_content);
static void handle_macro_content(char *line, char **macro_content);
static void write_or_expand_line(FILE *output_file, char *line);

/* This function allocates initial memory for storing macros.
 It should be called once at the start of the program.
 */
void init_macros(void) {
    macro_capacity = INITIAL_MACRO_CAPACITY;
    macros = (Macro*)calloc(macro_capacity, sizeof(Macro));
    if (macros == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for macros\n");
        exit(1);
    }
}

/* This helper function is used to validate macro names against known keywords.
 Parameters:
 1. word - the word to check
 2. list - the list of words to check against
 3. list_count - the number of words in the list
 Returns 1 if the word is in the list, 0 otherwise
 */
static int word_in_list(const char *word, const char *list[], int list_count) {
    int i;
    for (i = 0; i < list_count; i++) {
        if (strcmp(word, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* This function checks if the given word is not a reserved keyword or instruction.
 Parameters:
  word - the potential macro name to check
 Returns 1 if the word can be a macro name, 0 otherwise
 */
int can_be_macro_name(const char *word) {
    return !(word_in_list(word, group1, group1_count) ||
             word_in_list(word, group2, group2_count) ||
             word_in_list(word, group3, group3_count) ||
             word_in_list(word, instruction_words, instruction_words_count));
}

/* This function reads a line from the given file and allocates memory for it.
 It also removes newline characters.
 * Returns a dynamically allocated string containing the line, or NULL if EOF is reached
 */
static char* read_line(FILE *file) {
    char buffer[MAX_LINE_LENGTH];
    char *line;

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        return NULL;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    line = (char*)malloc(strlen(buffer) + 1);
    if (line == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for line\n");
        exit(1);
    }
    strcpy(line, buffer);
    return line;
}

/* Resize the macro array if needed */
static void resize_macro_array(void) {
    Macro* temp;
    if (macro_count >= macro_capacity) {
        macro_capacity *= 2;
        temp = (Macro*)realloc(macros, macro_capacity * sizeof(Macro));
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed for macros\n");
            exit(1);
        }
        macros = temp;
    }
}

/* This function searches for a macro with the given name in the macro array.
 * Returns a pointer to the found Macro, or NULL if not found  */
static Macro* find_macro(const char *name) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(name, macros[i].name) == 0) {
            return &macros[i];
        }
    }
    return NULL;
}

/* Handle the start of a macro definition
 It extracts the macro name and checks if it's valid.
 Parameters:
 1. line - the line containing the macro definition start
 2. macro_name - buffer to store the extracted macro name
 3. line_number - the current line number (for error reporting)
 Returns 1 if the macro start is valid, 0 otherwise
 */
static int handle_macro_start(char *line, char *macro_name, int line_number) {
    int scan_result;

    scan_result = sscanf(line, "%*s %s", macro_name);
    if (scan_result != 1 || !can_be_macro_name(macro_name)) {
        fprintf(stderr, "Error: Invalid macro name at line %d\n", line_number);
        return 0;
    }
    return 1;
}

/* This function finalizes a macro definition by adding it to the macro array.
 Parameters:
 1. macro_name - the name of the macro being defined
 2. macro_content - pointer to the string containing the macro content
 */
static void handle_macro_end(char *macro_name, char **macro_content) {
    resize_macro_array();
    strncpy(macros[macro_count].name, macro_name, MAX_MACRO_NAME - 1);
    macros[macro_count].name[MAX_MACRO_NAME - 1] = '\0';
    macros[macro_count].content = *macro_content;
    macro_count++;
    macro_name[0] = '\0';
    *macro_content = NULL;
}

/* This function adds a line to the content of a macro being defined. */
static void handle_macro_content(char *line, char **macro_content) {
    char* new_content;
    new_content = (char*)realloc(*macro_content, strlen(*macro_content) + strlen(line) + 2);
    if (new_content == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed for macro content\n");
        free(*macro_content);
        exit(1);
    }
    *macro_content = new_content;
    strcat(*macro_content, line);
    strcat(*macro_content, "\n");
}

/* This function checks if a line starts with a macro name and expands it if so.
   otherwise, it writes the line as-is to the output file */
static void write_or_expand_line(FILE *output_file, char *line) {
    char first_word[MAX_MACRO_NAME];
    Macro* current_macro;

    sscanf(line, "%s", first_word);
    current_macro = find_macro(first_word);
    if (current_macro != NULL) {
        fputs(current_macro->content, output_file);
    } else {
        fputs(line, output_file);
        fputc('\n', output_file);
    }
}

/* This function reads the input file, processes macros, and writes the result to the output file.
 It handles macro definitions and expansions */
static void process_macros(const char *input_name, const char *output_name) {
    FILE *input_file;
    FILE *output_file;
    char *line;
    int in_macro_definition = 0;
    char macro_name[MAX_MACRO_NAME] = {0};
    char* macro_content = NULL;
    int line_number = 0;

    input_file = fopen(input_name, "r");
    output_file = fopen(output_name, "w");

    if (input_file == NULL) {
        fprintf(stderr, "Error: Unable to open input file '%s'\n", input_name);
        return;
    }
    if (output_file == NULL) {
        fprintf(stderr, "Error: Unable to create output file '%s'\n", output_name);
        fclose(input_file);
        return;
    }

    while ((line = read_line(input_file)) != NULL) {
        line_number++;

        char first_word[MAX_MACRO_NAME];
        sscanf(line, "%s", first_word);

        if (strcmp(first_word, "macr") == 0) {
            if (in_macro_definition) {
                fprintf(stderr, "Error: Nested macro definition at line %d\n", line_number);
            } else if (handle_macro_start(line, macro_name, line_number)) {
                in_macro_definition = 1;
                macro_content = (char*)malloc(1);
                if (macro_content == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for macro content\n");
                    free(line);
                    fclose(input_file);
                    fclose(output_file);
                    return;
                }
                macro_content[0] = '\0';
            }
        } else if (strcmp(first_word, "endmacr") == 0) {
            if (!in_macro_definition) {
                fprintf(stderr, "Error: Unexpected 'endmacr' at line %d\n", line_number);
            } else {
                handle_macro_end(macro_name, &macro_content);
                in_macro_definition = 0;
            }
        } else if (in_macro_definition) {
            handle_macro_content(line, &macro_content);
        } else {
            write_or_expand_line(output_file, line);
        }
        free(line);
    }

    if (in_macro_definition) {
        fprintf(stderr, "Error: Macro '%s' not closed at end of file\n", macro_name);
        free(macro_content);
    }

    fclose(input_file);
    fclose(output_file);
}

/* This is the main function called from outside to process macros.
 It creates the .am file name and calls process_macros to do the actual work */
void replace_macros(const char *input_name) {
    char output_name[MAX_FILENAME_LENGTH];
    char *base_name;
    size_t base_name_length;

    base_name = strrchr(input_name, '/');
    if (base_name == NULL) {
        base_name = (char *)input_name;
    } else {
        base_name++;
    }
    base_name_length = strlen(base_name) - 3;

    strncpy(output_name, base_name, base_name_length);
    output_name[base_name_length] = '\0';
    strcat(output_name, ".am");

    process_macros(input_name, output_name);
}

/* Free all allocated memory for macros */
void free_macros(void) {
    int i;
    for (i = 0; i < macro_count; i++) {
        free(macros[i].content);
    }
    free(macros);
    macros = NULL;
    macro_count = 0;
    macro_capacity = 0;
}