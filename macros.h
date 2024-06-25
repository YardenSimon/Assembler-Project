// macros.h

#ifndef MACROS_H
#define MACROS_H

#define MAX_MACROS 100
#define MAX_MACRO_CONTENT 1000

// Structure to store macro definitions
typedef struct {
    char name[70];
    char content[MAX_MACRO_CONTENT];
} Macro;

// Function declarations
int is_valid_macro_name(const char *token);
void process_macro_definition(const char *macro_name, FILE *file);

// External variable to store macros
extern Macro macros[MAX_MACROS];
extern int macro_count;

#endif // MACROS_H
