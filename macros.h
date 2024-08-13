
#ifndef MACROS_H
#define MACROS_H
#include "globals.h"

/* Maximum length for a macro name */
#define MAX_MACRO_NAME MAX_LABEL_LENGTH

/* Structure to represent a macro */
typedef struct {
    char name[MAX_MACRO_NAME];
    char* content;
} Macro;

/* Initialize the macro storage */
void init_macros(void);

/* Check if a word can be a valid macro name */
int can_be_macro_name(const char *word);

/* Replace macros in the input file and generate .am file */
void replace_macros(const char *input_name);

/* Free all allocated memory for macros */
void free_macros(void);

#endif /* MACROS_H */