#include "errors.h"
#include "globals.h"
#include "symbol_table.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

static ErrorNode* error_list_head = NULL;
static ErrorNode* error_list_tail = NULL;
static int error_count = 0;

void init_error_handling(void)
{
    error_list_head = NULL;
    error_list_tail = NULL;
    error_count = 0;
}

void add_error(ErrorType type, const char* filename, int line, const char* format, ...)
{
    va_list args;
    ErrorNode* new_error;

    new_error = (ErrorNode*)malloc(sizeof(ErrorNode));
    if (new_error == NULL) {
        fprintf(stderr, "Fatal error: Unable to allocate memory for error handling\n");
        exit(1);
    }

    new_error->type = type;
    new_error->line = line;
    strncpy(new_error->filename, filename, sizeof(new_error->filename) - 1);
    new_error->filename[sizeof(new_error->filename) - 1] = '\0';

    va_start(args, format);
    vsnprintf(new_error->message, sizeof(new_error->message), format, args);
    va_end(args);

    new_error->next = NULL;

    if (error_list_tail == NULL) {
        error_list_head = new_error;
        error_list_tail = new_error;
    } else {
        error_list_tail->next = new_error;
        error_list_tail = new_error;
    }

    error_count++;
}

void print_errors(void)
{
    ErrorNode* current = error_list_head;

    while (current != NULL) {
        fprintf(stderr, "Error in file %s at line %d: ", current->filename, current->line);

        switch (current->type) {
            case ERROR_MACRO:
                fprintf(stderr, "Macro error: ");
                break;
            case ERROR_INVALID_LABEL:
                fprintf(stderr, "Invalid label: ");
                break;
            case ERROR_INVALID_INSTRUCTION:
                fprintf(stderr, "Invalid instruction: ");
                break;
            case ERROR_INVALID_MACRO_DEFINITION:
                fprintf(stderr, "Invalid macro definition: ");
                break;
            case ERROR_DUPLICATE_LABEL_DEFINITION:
                fprintf(stderr, "Duplicate label definition: ");
                break;
            case ERROR_ENTRY_EXTERN_CONFLICT:
                fprintf(stderr, "Entry/Extern conflict: ");
                break;
            case ERROR_SYMBOL_CONFLICT:
                fprintf(stderr, "Symbol conflict: ");
                break;
            case ERROR_RESERVED_WORD_AS_LABEL:
                fprintf(stderr, "Reserved word used as label: ");
                break;
            case ERROR_UNDEFINED_LABEL:
                fprintf(stderr, "Undefined label: ");
                break;
            case ERROR_INVALID_OPERAND:
                fprintf(stderr, "Invalid operand: ");
                break;
            case ERROR_FILE_NOT_FOUND:
                fprintf(stderr, "File not found: ");
                break;
            case ERROR_MEMORY_ALLOCATION:
                fprintf(stderr, "Memory allocation failed: ");
                break;
            default:
                fprintf(stderr, "Unknown error: ");
        }

        fprintf(stderr, "%s\n", current->message);
        current = current->next;
    }
}

int get_error_count(void)
{
    return error_count;
}

int has_errors(void)
{
    return error_count > 0;
}

void free_error_handling(void)
{
    ErrorNode* current = error_list_head;
    ErrorNode* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    error_list_head = NULL;
    error_list_tail = NULL;
    error_count = 0;
}

int is_reserved_word(const char* word)
{
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(word, OPCODE_NAMES[i]) == 0) {
            return 1;
        }
    }
    for (i = 0; i < NUM_DIRECTIVES; i++) {
        if (strcmp(word, DIRECTIVE_NAMES[i]) == 0) {
            return 1;
        }
    }
    /* Check for register names (r0-r7) */
    if (word[0] == 'r' && word[1] >= '0' && word[1] <= '7' && word[2] == '\0') {
        return 1;
    }
    return 0;
}

int is_label(const char* symbol)
{
    return get_symbol_by_name(symbol) != NULL;
}

int is_macro(const char* symbol)
{
    /* Assuming you have a function in macros.h to check for macro existence */
    return macro_exists(symbol);
}

int is_entry_extern_conflict(const char* symbol)
{
    Symbol* sym = get_symbol_by_name(symbol);
    return sym && sym->is_entry && sym->is_external;
}

int is_valid_label_name(const char* label)
{
    int i;
    if (!isalpha((int)label[0])) {
        return 0;
    }
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum((int)label[i])) {
            return 0;
        }
    }
    return strlen(label) <= MAX_LABEL_LENGTH;
}

int is_extern(const char* symbol)
{
    Symbol* sym = get_symbol_by_name(symbol);
    return sym && sym->is_external;
}

int is_entry(const char* symbol)
{
    Symbol* sym = get_symbol_by_name(symbol);
    return sym && sym->is_entry;
}
