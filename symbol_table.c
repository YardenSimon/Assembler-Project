/* symbol_table.c */

#include "utils.h"
#include "symbol_table.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INITIAL_SYMBOL_CAPACITY 10
int symbol_exists(const char* name);
Symbol* symbol_table = NULL;
static int symbol_capacity = 0;
int symbol_count = 0;


void init_symbol_table(void) {
    symbol_capacity = INITIAL_SYMBOL_CAPACITY;
    symbol_table = (Symbol*)calloc(symbol_capacity, sizeof(Symbol));
    if (symbol_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }
    symbol_count = 0;
}

void add_symbol(const char* name, int address) {
    Symbol* temp;

    printf("DEBUG: Adding symbol: %s with address: %d\n", name, address);

    if (symbol_exists(name)) {
        printf("DEBUG: Symbol %s already exists. Not adding again.\n", name);
        return;
    }

    if (symbol_count >= symbol_capacity) {
        symbol_capacity *= 2;
        temp = (Symbol*)realloc(symbol_table, symbol_capacity * sizeof(Symbol));
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed for symbol table\n");
            exit(1);
        }
        symbol_table = temp;
        printf("DEBUG: Symbol table resized. New capacity: %d\n", symbol_capacity);
    }

    strncpy(symbol_table[symbol_count].name, name, MAX_SYMBOL_LENGTH - 1);
    symbol_table[symbol_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
    symbol_table[symbol_count].address = address;


    symbol_count++;

    printf("DEBUG: Symbol added. New symbol count: %d\n", symbol_count);
}

Symbol* get_symbol_by_name(const char* name) {
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

Symbol* get_symbol_by_address(int address) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (symbol_table[i].address == address) {
            return &symbol_table[i];
        }
    }
    return NULL;
}

Symbol* get_symbol_by_index(int index) {
    if (index >= 0 && index < symbol_count) {
        return &symbol_table[index];
    }
    return NULL;
}

// void mark_external(const char* name) {
//     Symbol* symbol = get_symbol_by_name(name);
//     if (symbol != NULL) {
//         symbol->is_external = 1;
//         symbol->address = 0;  /* External symbols have address 0 */
//     } else {
//         add_symbol(name, 0);
//         symbol_table[symbol_count - 1].is_external = 1;
//     }
//     printf("DEBUG: Marked symbol %s as external\n", name);
// }

void free_symbol_table(void) {
    free(symbol_table);
    symbol_table = NULL;
    symbol_count = 0;
    symbol_capacity = 0;
}

int get_symbol_count(void) {
    return symbol_count;
}

void print_symbol_table(void) {
    int i;
    printf("\n--- Symbol Table ---\n");
    printf("%-20s %-10s %-10s %-10s\n", "Name", "Address", "External", "Entry");
    printf("----------------------------------------\n");
    for (i = 0; i < get_symbol_count(); i++) {
        Symbol* symbol = get_symbol_by_index(i);
        if (symbol != NULL) {
            printf("%-20s %-10d %-10s %-10s\n",
                   symbol->name,
                   symbol->address,
                   symbol->is_external ? "Yes" : "No",
                   symbol->is_entry ? "Yes" : "No");
        }
    }
    printf("----------------------------------------\n");
}

int symbol_exists(const char* name) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            printf("DEBUG: Symbol %s already exists in table\n", name);
            return 1;
        }
    }
    return 0;
}
