#ifndef ERRORS_H
#define ERRORS_H

/* Define error types */
typedef enum {
    ERROR_MACRO,
    ERROR_INVALID_LABEL,
    ERROR_INVALID_INSTRUCTION,
    ERROR_INVALID_MACRO_DEFINITION,
    ERROR_DUPLICATE_LABEL_DEFINITION,
    ERROR_ENTRY_EXTERN_CONFLICT,
    ERROR_SYMBOL_CONFLICT,
    ERROR_RESERVED_WORD_AS_LABEL,
    ERROR_UNDEFINED_LABEL,
    ERROR_INVALID_OPERAND,
    ERROR_FILE_NOT_FOUND,
    ERROR_MEMORY_ALLOCATION
} ErrorType;

/* Typedef for the error structure */
typedef struct ErrorNode {
    ErrorType type;
    int line;
    char message[256];  /* Fixed-size buffer for the error message */
    char filename[256]; /* To store the filename where the error occurred */
    struct ErrorNode* next;
} ErrorNode;

/* Initialize the error handling system */
void init_error_handling(void);

/* Add an error to the list */
void add_error(ErrorType type, const char* filename, int line, const char* format, ...);

/* Print all errors */
void print_errors(void);

/* Get the total number of errors encountered */
int get_error_count(void);

/* Check if there are any errors */
int has_errors(void);

/* Free any resources used by the error handling system */
void free_error_handling(void);

/* Check if a word is a reserved word (instruction, register, etc.) */
int is_reserved_word(const char* word);

/* Check if a symbol is already defined as a label */
int is_label_name(const char* symbol);

/* Check if a symbol is already defined as a macro */
int is_macro(const char* symbol);

/* Check if a symbol is defined as both entry and extern */
int is_entry_extern_conflict(const char* symbol);

/* Validate a label name */
int is_valid_label_name(const char* label);

/* Check if a symbol is defined as extern */
int is_extern(const char* symbol);

/* Check if a symbol is defined as entry */
int is_entry(const char* symbol);

#endif /* ERRORS_H */