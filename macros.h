#ifndef MACROS_H
#define MACROS_H
#include "globals.h"
#define INITIAL_MACRO_CAPACITY 10

#define MAX_MACRO_NAME MAX_LABEL_LENGTH

typedef struct {
    char name[MAX_MACRO_NAME];
    char* content;
} Macro;

/* Initializes the macro system by allocating initial memory for the macro array.
 * Sets the initial capacity for storing macros to avoid frequent reallocations.
 */
void init_macros(void);

/* Checks if a given word can be used as a macro name.
 * Returns 1 if the word can be a macro name, 0 otherwise.
 * It ensures that the word does not conflict with opcodes or directives.
 */
int can_be_macro_name(const char *word);

/* Replaces macros in the input file and generates an output file with the
 * macros expanded. The output file has a different extension for clarity.
 */
void replace_macros(const char *input_name);

/* Checks if a macro with a given name exists
 * It looks through all defined macros
 * Returns 1 if the macro exists, 0 if not
*/
int is_macro_defined(const char* name);

/* Frees all memory allocated for macros. This includes freeing each macro's content
 * and the macro array itself. It also resets the macro-related variables to their
 * initial states.
 */
void free_macros(void);

#endif