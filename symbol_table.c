/* symbol_table.c */

#include "symbol_table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Symbol* symbol_table = NULL;
int symbol_count = 0;
int symbol_capacity = 0;
int current_symbol_index = 0;

/* Initialize the symbol table with an initial capacity */
void init_symbol_table(void) {
    int initial_capacity = 10;  /* Start with space for 10 symbols */

    symbol_capacity = initial_capacity;
    symbol_table = (Symbol*)calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
    symbol_count = 0;
    current_symbol_index = 0;
}

/* Add a new symbol to the symbol table */
void add_symbol(const char* name, int address) {
    Symbol *temp;

    if (symbol_count >= symbol_capacity) {
        /* Double the capacity if we've reached the limit */
        symbol_capacity *= 2;
        temp = (Symbol*)realloc(symbol_table, symbol_capacity * sizeof(Symbol));
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

/* Look up a symbol by name and return its address */
int lookup_symbol(const char* name) {
    int i;

    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1; /* Symbol not found */
}

/* Get a symbol by its address */
Symbol* get_symbol_by_address(int address) {
    int i;

    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].address == address) {
            return &symbol_table[i];
        }
    }
    return NULL;  /* Symbol not found */
}

/* Check if there are any external symbols */
bool has_external_symbols(void) {
    int i;

    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_external) {
            return true;
        }
    }
    return false;
}

/* Check if there are any entry symbols */
bool has_entry_symbols(void) {
    int i;

    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_entry) {
            return true;
        }
    }
    return false;
}

/* Get the first symbol in the table */
Symbol* get_first_symbol(void) {
    current_symbol_index = 0;
    return (symbol_count > 0) ? &symbol_table[0] : NULL;
}

/* Get the next symbol in the table */
Symbol* get_next_symbol(void) {
    current_symbol_index++;
    if (current_symbol_index < symbol_count) {
        return &symbol_table[current_symbol_index];
    }
    return NULL;
}

/* Mark a symbol as external */
void mark_external(const char* name) {
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

/* Mark a symbol as an entry point */
void mark_entry(const char* name) {
    int i;

    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].is_entry = 1;
            return;
        }
    }
    fprintf(stderr, "Error: Trying to mark non-existent symbol '%s' as entry\n", name);
}

/* Free the memory allocated for the symbol table */
void free_symbol_table(void) {
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
    current_symbol_index = 0;
}