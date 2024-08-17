/* entry_extern.h */

#ifndef ENTRY_EXTERN_H
#define ENTRY_EXTERN_H

#include "globals.h"

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
} EntrySymbol;

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
} ExternSymbol;

#define MAX_ENTRIES 100
#define MAX_EXTERNS 100

extern EntrySymbol entries[MAX_ENTRIES];
extern int entry_count;

extern ExternSymbol externs[MAX_EXTERNS];
extern int extern_count;

void add_entry(const char* name);
void add_extern(const char* name);
void init_entry_extern(void);
void set_entry_address(const char* name, int address);

#endif /* ENTRY_EXTERN_H */