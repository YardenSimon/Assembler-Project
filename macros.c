#include "macros.h"
#include "utils.h"
#include "globals.h"
#include "errors.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INITIAL_MACRO_CAPACITY 10
Macro *macros = NULL;
int macro_count = 0;
int macro_capacity = 0;

static char *read_line(FILE *file);
static void resize_macro_array(void);
static Macro *find_macro(const char *name);
static void process_macros(const char *input_name, const char *output_name);
static int handle_macro_start(char *line, char *macro_name, int line_number);
static void handle_macro_end(char *macro_name, char **macro_content);
static void handle_macro_content(char *line, char **macro_content);
static void write_or_expand_line(FILE *output_file, char *line);

/*
 * Initializes the macro system by allocating initial memory for the macro array.
 * Sets the initial capacity for storing macros to avoid frequent reallocations.
 */
void init_macros(void) {
    macro_capacity = INITIAL_MACRO_CAPACITY;
    macros = (Macro *) safe_malloc(macro_capacity * sizeof(Macro));
}

/*
 * Checks if a given word can be used as a macro name.
 * Returns 1 if the word can be a macro name, 0 otherwise.
 * It ensures that the word does not conflict with opcodes or directives.
 */
int can_be_macro_name(const char *word) {
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(word, OPCODE_NAMES[i]) == 0) return 0;
    }
    for (i = 0; i < NUM_DIRECTIVES; i++) {
        if (strcmp(word, DIRECTIVE_NAMES[i]) == 0) return 0;
    }
    return 1;
}

/*
 * Reads a line from the given file, removes any trailing newlines,
 * and returns it as a dynamically allocated string.
 * This function ensures the input line is correctly formatted for further processing.
 */
static char *read_line(FILE *file) {
    char buffer[MAX_LINE_LENGTH];
    char *line;

    if (safe_fgets(buffer, sizeof(buffer), file) == NULL) {
        return NULL;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    line = (char *) safe_malloc(strlen(buffer) + 1);
    strcpy(line, buffer);
    return line;
}

/*
 * Resizes the macro array when its capacity is exceeded.
 * This function doubles the array's capacity, reallocating memory to store more macros.
 */
static void resize_macro_array(void) {
    if (macro_count >= macro_capacity) {
        int new_capacity = macro_capacity * 2;
        Macro *new_macros = (Macro *) safe_malloc(new_capacity * sizeof(Macro));
        memcpy(new_macros, macros, macro_count * sizeof(Macro));
        free(macros);
        macros = new_macros;
        macro_capacity = new_capacity;
    }
}

/*
 * Searches for a macro by name in the macro array.
 * Returns a pointer to the Macro structure if found, or NULL if not found.
 */
static Macro *find_macro(const char *name) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(name, macros[i].name) == 0) {
            return &macros[i];
        }
    }
    return NULL;
}

/*
 * Handles the start of a macro definition.
 * This function extracts the macro name from the line and validates it.
 * Returns 1 if the macro name is valid, 0 otherwise.
 */
static int handle_macro_start(char *line, char *macro_name, int line_number) {
    int macro_result;

    macro_result = sscanf(line, "%*s %s", macro_name);
    if (macro_result != 1 || !can_be_macro_name(macro_name)) {
        /* Invalid macro name */
        add_error(ERROR_INVALID_MACRO_DEFINITION, current_filename, line_number, "Invalid macro name");
        return 0;
    }
    return 1;
}

/*
 * This function finalizes the definition of a macro by storing its content and
 * adding it to the global macro array. It ensures there is enough space in the
 * array by resizing it if necessary. The macro's name and content are copied
 * into the array, the macro count is incremented, and the macro name and content
 * pointers are reset for future use.
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

/*
 * This function manages the content of a macro during its definition by appending
 * each line to the existing macro content. It dynamically resizes the content string
 * as needed to accommodate the new lines, ensuring that the final macro content is
 * correctly accumulated and stored.
 */

static void handle_macro_content(char *line, char **macro_content) {
    size_t current_length = *macro_content ? strlen(*macro_content) : 0;
    size_t line_length = strlen(line);
    size_t new_length = current_length + line_length + 2;
    char *new_content = (char *) safe_malloc(new_length);

    if (current_length > 0) {
        strcpy(new_content, *macro_content);
        free(*macro_content);
    } else {
        new_content[0] = '\0';
    }

    strcat(new_content, line);
    strcat(new_content, "\n");
    *macro_content = new_content;
}

/*
 * This function writes a line to the output file. If the line starts with a macro name,
 * it expands the macro by writing its content instead of the line. If the line doesn't
 * correspond to a macro, it writes the line as it is.
 */
static void write_or_expand_line(FILE *output_file, char *line) {
    char first_word[MAX_MACRO_NAME];
    Macro *current_macro;

    sscanf(line, "%s", first_word);
    current_macro = find_macro(first_word);
    if (current_macro != NULL) {
        fputs(current_macro->content, output_file);
    } else {
        fputs(line, output_file);
        fputc('\n', output_file);
    }
}

/*
 * This function processes the macros in the input file. It reads each line, handles
 * macro definitions and expansions, and writes the expanded result to the output file.
 * The function ensures correct macro processing, including handling errors like nested
 * or unclosed macro definitions.
 */
static void process_macros(const char *input_name, const char *output_name) {
    FILE *input_file;
    FILE *output_file;
    char *line;
    /* Flag to check if inside a macro definition */
    int in_macro_definition = 0;
    char macro_name[MAX_MACRO_NAME] = {0};
    char *macro_content = NULL;
    int line_number = 0;
    char first_word[MAX_MACRO_NAME];

    input_file = safe_fopen(input_name, "r");
    output_file = safe_fopen(output_name, "w");

    while ((line = read_line(input_file)) != NULL) {
        line_number++;
        sscanf(line, "%s", first_word);
        /*FIRST WORD WAS HERE BEFORE!!!!!!!!!!!!!!!!!!*/
        if (strcmp(first_word, "macr") == 0) {
            if (in_macro_definition) {
                add_error(ERROR_INVALID_MACRO_DEFINITION, input_name, line_number, "Nested macro definition");
            } else if (handle_macro_start(line, macro_name, line_number)) {
                in_macro_definition = 1;
                macro_content = (char *) safe_malloc(1);
                macro_content[0] = '\0';
            }
        } else if (strcmp(first_word, "endmacr") == 0) {
            if (!in_macro_definition) {
                add_error(ERROR_INVALID_MACRO_DEFINITION, input_name, line_number, "Unexpected 'endmacr'");
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
        add_error(ERROR_INVALID_MACRO_DEFINITION, input_name, line_number, "Macro '%s' not closed at end of file", macro_name);
        free(macro_content);
    }

    fclose(input_file);
    fclose(output_file);
}

/*
 * Replaces macros in the input file and generates an output file with the
 * macros expanded. The output file has a different extension for clarity.
 */
void replace_macros(const char *input_name) {
    BaseFilename base_filename = get_base_filename(input_name);
    char output_name[MAX_FILENAME_LENGTH];

    strncpy(output_name, base_filename.name, base_filename.length);
    output_name[base_filename.length] = '\0';
    strcat(output_name, MACRO_FILE_EXT);

    process_macros(input_name, output_name);

    free(base_filename.name);
}

int is_macro_defined(const char* name) {
    int i;
    for (i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Frees all memory allocated for macros. This includes freeing each macro's content
 * and the macro array itself. It also resets the macro-related variables to their
 * initial states.
 */
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
