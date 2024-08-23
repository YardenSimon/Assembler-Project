#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "globals.h"

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
    int is_external;
    int is_entry;
} Symbol;

extern Symbol* symbol_table;
extern int symbol_count;

/* Initializes the symbol table with an initial capacity.
 * Allocates memory for the symbol table and sets initial values. */
void init_symbol_table();

/* Adds a new symbol to the symbol table.
 * If the symbol already exists, it does nothing.
 * Expands the symbol table if necessary. */
void add_symbol(const char* name, int address);

/* Retrieves a symbol from the symbol table by its name.
 * Returns a pointer to the symbol if found, or NULL if not found. */
Symbol* get_symbol_by_name(const char* name);

/* Frees the memory allocated for the symbol table.
 * Resets the symbol table pointer and counts. */
void free_symbol_table(void);

/* Returns the number of symbols currently in the symbol table. */
int get_symbol_count(void);

#endif