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


void init_symbol_table();
void add_symbol(const char* name, int address);
Symbol* get_symbol_by_name(const char* name);
void free_symbol_table(void);
int get_symbol_count(void);
Symbol* get_symbol_by_index(int index);

void print_symbol_table(void);

#endif /* SYMBOL_TABLE_H */