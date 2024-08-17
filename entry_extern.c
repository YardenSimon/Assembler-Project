/* entry_extern.c */

#include "entry_extern.h"
#include <string.h>
#include <stdio.h>

EntrySymbol entries[MAX_ENTRIES];
int entry_count = 0;

ExternSymbol externs[MAX_EXTERNS];
int extern_count = 0;

void init_entry_extern(void) {
    entry_count = 0;
    extern_count = 0;
}

void add_entry(const char* name) {
    if (entry_count < MAX_ENTRIES) {
        strncpy(entries[entry_count].name, name, MAX_SYMBOL_LENGTH - 1);
        entries[entry_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        entries[entry_count].address = -1;  /* Address will be set later */
        entry_count++;
    } else {
        fprintf(stderr, "Error: Too many entry symbols\n");
    }
}

void add_extern(const char* name) {
    if (extern_count < MAX_EXTERNS) {
        strncpy(externs[extern_count].name, name, MAX_SYMBOL_LENGTH - 1);
        externs[extern_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        extern_count++;
    } else {
        fprintf(stderr, "Error: Too many extern symbols\n");
    }
}

void set_entry_address(const char* name, int address) {
    int i;
    for (i = 0; i < entry_count; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            entries[i].address = address;
            return;
        }
    }
    fprintf(stderr, "Error: Entry symbol '%s' not found\n", name);
}