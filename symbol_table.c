#include "symbol_table.h"
#include <string.h>
#include <stdio.h>

Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

void add_symbol(const char* name, int address) {
    if (symbol_count < MAX_SYMBOLS) {
        strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
        symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        symbol_table[symbol_count].address = address;
        symbol_table[symbol_count].is_external = 0;
        symbol_table[symbol_count].is_entry = 0;
        symbol_count++;
    } else {
        fprintf(stderr, "Error: Symbol table is full\n");
    }
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