/* symbol_table.h */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOL_LENGTH 80

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
    int is_external;
    int is_entry;
} Symbol;

void init_symbol_table();
void add_symbol(const char* name, int address);
int lookup_symbol(const char* name);
void mark_external(const char* name);
void mark_entry(const char* name);
void free_symbol_table();

#endif /* SYMBOL_TABLE_H */