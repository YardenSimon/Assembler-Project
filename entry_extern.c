#include "entry_extern.h"
#include "utils.h"
#include "errors.h"
#include <string.h>
#include <stdio.h>

/* Array of entry symbols */
EntrySymbol entries[MAX_ENTRIES];
int entry_count = 0;
/* Array for extern symbols */
ExternSymbol externs[MAX_EXTERNS];
int extern_count = 0;

ExternUse *extern_uses = NULL;
int extern_use_count = 0;


void init_entry_extern(void) {
    entry_count = 0;
    extern_count = 0;
}


void init_extern_uses(void) {
    extern_uses = (ExternUse *)safe_malloc(MAX_EXTERN_USES * sizeof(ExternUse));
    extern_use_count = 0;
}


void add_entry(const char* name) {
    if (entry_count < MAX_ENTRIES) {
        strncpy(entries[entry_count].name, name, MAX_SYMBOL_LENGTH - 1);
        entries[entry_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        entries[entry_count].address = -1;
        entry_count++;
    } else {
        add_error(ERROR_INVALID_OPERAND, current_filename, -1, "Too many entry symbols");
    }
}



void add_extern(const char* name) {
    if (extern_count < MAX_EXTERNS) {
        strncpy(externs[extern_count].name, name, MAX_SYMBOL_LENGTH - 1);
        externs[extern_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        extern_count++;
    } else {
        add_error(ERROR_INVALID_OPERAND, current_filename, -1, "Too many extern symbols");
    }
}


void add_extern_use(const char* name, int address) {
    if (extern_use_count < MAX_EXTERN_USES) {
        strncpy(extern_uses[extern_use_count].name, name, MAX_SYMBOL_LENGTH - 1);
        extern_uses[extern_use_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        extern_uses[extern_use_count].address = address;
        extern_use_count++;
    } else {
        add_error(ERROR_INVALID_OPERAND, current_filename, -1, "Too many external symbol uses");
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
    add_error(ERROR_UNDEFINED_LABEL, current_filename, -1, "Entry symbol '%s' not found", name);
}


void free_entry_extern_resources(void) {
    if (extern_uses != NULL) {
        free(extern_uses);
        extern_uses = NULL;
    }
    entry_count = 0;
    extern_count = 0;
    extern_use_count = 0;

}