#include "symbol_table.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const int INITIAL_SYMBOL_CAPACITY = 10;

int symbol_exists(const char *name);

Symbol *symbol_table = NULL;
static int symbol_capacity = 0;
int symbol_count = 0;

/*
 * Initializes the symbol table with an initial capacity.
 * Allocates memory for the symbol table and sets initial values.
 */
void init_symbol_table() {
    symbol_capacity = INITIAL_SYMBOL_CAPACITY;
    symbol_table = (Symbol *) calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
    symbol_count = 0;
}

/*
 * Adds a new symbol to the symbol table.
 * If the symbol already exists, it does nothing.
 * Expands the symbol table if necessary.
 */
void add_symbol(const char *name, int address) {
    Symbol *temp;
    if (symbol_exists(name)) {
        printf("DEBUG: Symbol %s already exists. Not adding again.\n", name);
        return;
    }
    printf("DEBUG: Adding symbol: %s with address: %d\n", name, address);

    if (symbol_count >= symbol_capacity) {
        symbol_capacity *= 2;
        temp = (Symbol *) realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed for symbol table\n");
            exit(1);
        }
        symbol_table = temp;
        printf("DEBUG: Symbol table resized. New capacity: %d\n", symbol_capacity);
    }

    /* Add the new symbol to the table */
    strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
    symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
    symbol_table[symbol_count].address = address;

    symbol_count++;

    printf("DEBUG: Symbol added. New symbol count: %d\n", symbol_count);
}

/*
 * Retrieves a symbol from the symbol table by its name.
 * Returns a pointer to the symbol if found, or NULL if not found.
 */
Symbol *get_symbol_by_name(const char *name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            printf("DEBUG: Found symbol %s at index %d\n", name, i);
            return &symbol_table[i];
        }
    }
    printf("DEBUG: Symbol %s not found in table\n", name);
    return NULL;
}

/*
 * Retrieves a symbol from the symbol table by its address.
 * Returns a pointer to the symbol if found, or NULL if not found.
 */
Symbol *get_symbol_by_address(int address) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].address == address) {
            return &symbol_table[i];
        }
    }
    return NULL;
}

/*
 * Retrieves a symbol from the symbol table by its index.
 * Returns a pointer to the symbol if index is valid, or NULL if invalid.
 */
Symbol *get_symbol_by_index(int index) {
    if (index >= 0 && index < symbol_count) {
        return &symbol_table[index];
    }
    return NULL;
}

/*
 * Frees the memory allocated for the symbol table.
 * Resets the symbol table pointer and counts.
 */
void free_symbol_table(void) {
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
}

/* Returns the number of symbols currently in the symbol table. */
int get_symbol_count(void) {
    return symbol_count;
}

/* DEBUG PRINT - DELETE LATER!!!!!!!!!!!!!!!!!!!*/
void print_symbol_table(void) {
    int i;
    printf("\n--- Symbol Table ---\n");
    printf("%-*s %-10s %-10s %-10s\n", MAX_SYMBOL_LENGTH, "Name", "Address", "External", "Entry");
    printf("----------------------------------------\n");
    for (i = 0; i < get_symbol_count(); i++) {
        Symbol *symbol = get_symbol_by_index(i);
        if (symbol != NULL) {
            printf("%-*s %-10d %-10s %-10s\n",
                   MAX_SYMBOL_LENGTH,
                   symbol->name,
                   symbol->address,
                   symbol->is_external ? "Yes" : "No",
                   symbol->is_entry ? "Yes" : "No");
        }
    }
    printf("----------------------------------------\n");
}

/*
 * Checks if a symbol with the given name already exists in the symbol table.
 * Returns 1 if exists, 0 otherwise.
 */
int symbol_exists(const char *name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            printf("DEBUG: Symbol %s already exists in table\n", name);
            return 1;
        }
    }
    return 0;
}
