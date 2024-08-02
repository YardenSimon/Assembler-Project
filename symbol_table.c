/* symbol_table.c */

#include "symbol_table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Symbol* symbol_table = NULL;
int symbol_count = 0;
int symbol_capacity = 0;

void init_symbol_table() {
    symbol_capacity = 10;  /* Start with space for 10 symbols */
    symbol_table = (Symbol*)calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
}

void add_symbol(const char* name, int address) {
    if (symbol_count >= symbol_capacity) {
        symbol_capacity *= 2;
        Symbol* temp = (Symbol*)realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed for symbol table\n");
            exit(1);
        }
        symbol_table = temp;
    }

    strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
    symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
    symbol_table[symbol_count].address = address;
    symbol_table[symbol_count].is_external = 0;
    symbol_table[symbol_count].is_entry = 0;
    symbol_count++;
}

int lookup_symbol(const char* name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1; /* Symbol not found */
}

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

void free_symbol_table() {
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
}