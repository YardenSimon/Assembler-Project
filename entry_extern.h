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
    int address;
} ExternSymbol;

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
} ExternUse;

#define MAX_ENTRIES 100
#define MAX_EXTERNS 100
#define MAX_EXTERN_USES 1000


extern EntrySymbol entries[MAX_ENTRIES];
extern int entry_count;

extern ExternSymbol externs[MAX_EXTERNS];
extern int extern_count;

extern ExternUse *extern_uses;
extern int extern_use_count;

void add_entry(const char* name);
void add_extern(const char* name);
void init_entry_extern(void);
void init_extern_uses(void);
void set_entry_address(const char* name, int address);
void add_extern_use(const char* name, int address);
void free_entry_extern_resources(void);


#endif /* ENTRY_EXTERN_H */