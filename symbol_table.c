/* symbol_table.c */

#include "symbol_table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Symbol* symbol_table = NULL;
int symbol_count = 0;
int symbol_capacity = 0;

void init_symbol_table() {
    /* Initialize the symbol table with an initial capacity */
    symbol_capacity = 10;  /* Start with space for 10 symbols */
    symbol_table = (Symbol*)calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
}

void add_symbol(const char* name, int address) {
    /* Add a new symbol to the symbol table */
    if (symbol_count >= symbol_capacity) {
        /* Double the capacity if we've reached the limit */
        symbol_capacity *= 2;
        Symbol *temp = (Symbol *) realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed for symbol table\n");
            exit(1);
        }
        symbol_table = temp;
    }

    /* Copy the symbol name and set its properties */
    strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
    symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
    symbol_table[symbol_count].address = address;
    symbol_table[symbol_count].is_external = 0;
    symbol_table[symbol_count].is_entry = 0;
    symbol_count++;
}

int lookup_symbol(const char* name) {
    /* Look up a symbol by name and return its address */
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1; /* Symbol not found */
}

Symbol* get_symbol_by_address(int address) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].address == address) {
            return &symbol_table[i];
        }
    }
    return NULL;  /* Symbol not found */
}

bool has_external_symbols(void) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_external) {
            return true;
        }
    }
    return false;
}

bool has_entry_symbols(void) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_entry) {
            return true;
        }
    }
    return false;
}

Symbol* get_first_symbol(void) {
    current_symbol_index = 0;
    return (symbol_count > 0) ? &symbol_table[0] : NULL;
}

Symbol* get_next_symbol(void) {
    current_symbol_index++;
    if (current_symbol_index < symbol_count) {
        return &symbol_table[current_symbol_index];
    }
    return NULL;
}

void mark_external(const char* name) {
    /* Mark a symbol as external */
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].is_external = 1;
            return;
        }
    }
    /* If symbol not found, add it as external */
    add_symbol(name, 0);
    symbol_table[symbol_count - 1].is_external = 1;
}

void mark_entry(const char* name) {
    /* Mark a symbol as an entry point */
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].is_entry = 1;
            return;
        }
    }
    fprintf(stderr, "Error: Trying to mark non-existent symbol '%s' as entry\n", name);
}

int has_external_symbols(void) {
    /* Check if there are any external symbols in the symbol table */
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_external) {
            return 1; /* Found at least one external symbol */
        }
    }
    return 0; /* No external symbols found */
}

int has_entry_symbols(void) {
    /* Check if there are any entry symbols in the symbol table */
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_entry) {
            return 1; /* Found at least one entry symbol */
        }
    }
    return 0; /* No entry symbols found */
}

void free_symbol_table() {
    /* Free the memory allocated for the symbol table */
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
}