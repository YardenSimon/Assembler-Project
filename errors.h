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

/* Initializes the error handling system by setting up an empty linked list
*  for storing errors. This function should be called at the start of the
*  assembly process to ensure a clean slate for error tracking.
*/
void init_error_handling(void);

/* This function adds a new error to the list of errors. It does this by creating a new error entry,
 * filling it with the provided details (like the type of error, file name, and line number),
 * and then adding it to the list.
 * The function can handle different error messages by using flexible arguments.
 * If there's a problem with memory allocation, the program will stop and show an error message.
 */
void add_error(ErrorType type, const char* filename, int line, const char* format, ...);

/* This function prints all the errors that have been collected.
 * It goes through the list of errors, formats each one, and prints it to the standard error output (stderr).
 * Before printing, it converts the error type into a readable message.
*/
void print_errors(void);

/* Check if there are any errors */
int has_errors(void);

/* This function releases all the memory used for error handling.
 * It should be called at the end of the assembly process to clean up.
 * The function goes through the list of errors, frees each one, and resets all the error-related variables.*/
void free_error_handling(void);

/* This function checks if a specific word is a reserved word in the assembly language, like an opcode, directive, or register name.
 * The function compares the word to a list of opcodes, directives, and register names (like r0 to r7).
 */
int is_reserved_word(const char* word);

/* Check if a symbol is defined as a label in the symbol table */
int is_label_name(const char* symbol);

/* Check if a symbol is defined as a macro */
int is_macro(const char* symbol);


/* This function checks if a string is a valid label name based on assembly language rules.
 * A valid label must start with a letter, only include letters and numbers,
 * and not be longer than the allowed maximum length. */
int is_valid_label_name(const char* label);


#endif