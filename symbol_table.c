#include "symbol_table.h"
#include "errors.h"
#include <string.h>
#include <stdlib.h>

static const int INITIAL_SYMBOL_CAPACITY = 10;

/* Checks if a symbol with the given name already exists in the symbol table.
 * Returns 1 if exists, 0 otherwise. */
int symbol_exists(const char *name);

Symbol *symbol_table = NULL;
static int symbol_capacity = 0;
int symbol_count = 0;


void init_symbol_table() {
    symbol_capacity = INITIAL_SYMBOL_CAPACITY;
    symbol_table = (Symbol *) calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        add_error(ERROR_MEMORY_ALLOCATION, current_filename, -1, "Memory allocation failed for symbol table");
        exit(1);
    }
    symbol_count = 0;
}


void add_symbol(const char *name, int address) {
    Symbol *temp;
    if (symbol_exists(name)) {
        return;
    }

    if (symbol_count >= symbol_capacity) {
        symbol_capacity *= 2;
        temp = (Symbol *) realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if (temp == NULL) {
            add_error(ERROR_MEMORY_ALLOCATION, current_filename, -1, "Memory reallocation failed for symbol table");
            exit(1);
        }
        symbol_table = temp;
    }
    strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
    symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
    symbol_table[symbol_count].address = address;

    symbol_count++;
}


Symbol *get_symbol_by_name(const char *name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return &symbol_table[i];
        }
    }
    return NULL;
}

void free_symbol_table(void) {
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
}


int get_symbol_count(void) {
    return symbol_count;
}


int symbol_exists(const char *name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}
