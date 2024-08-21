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

/*
 * Initialize entry and extern symbol counts to zero.
 * This function is called at the start of the program
 * to ensure that the symbol tables are empty before use.
 */
void init_entry_extern(void) {
    entry_count = 0;
    extern_count = 0;
}

/*
 * Initialize memory to save all uses of externals and counts them.
 * This function is called at the start of the seconds pass
 * so at the end of it we can create the .ext file.
 */
void init_extern_uses(void) {
    extern_uses = (ExternUse *)safe_malloc(MAX_EXTERN_USES * sizeof(ExternUse));
    extern_use_count = 0;
}

/*
 * Add a new entry symbol to the entries array.
 * The function checks if there is space for more symbols.
 * If space is available, it copies the symbol name into the array
 * and sets the address to -1, indicating that it has not been assigned yet.
 */
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


/*
 * Add a new extern symbol to the externs array.
 * The function checks if there is space for more symbols.
 * If space is available, it copies the symbol name into the array.
 */
void add_extern(const char* name) {
    if (extern_count < MAX_EXTERNS) {
        strncpy(externs[extern_count].name, name, MAX_SYMBOL_LENGTH - 1);
        externs[extern_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        extern_count++;
        printf("DEBUG: Added extern %s, total externs: %d\n", name, extern_count);
    } else {
        add_error(ERROR_INVALID_OPERAND, current_filename, -1, "Too many extern symbols");
    }
}

/* Adds an occurrence of an extern to the exter uses array*/
void add_extern_use(const char* name, int address) {
    if (extern_use_count < MAX_EXTERN_USES) {
        strncpy(extern_uses[extern_use_count].name, name, MAX_SYMBOL_LENGTH - 1);
        extern_uses[extern_use_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        extern_uses[extern_use_count].address = address;
        extern_use_count++;
        printf("DEBUG: Added extern use: %s %04d\n", name, address);
    } else {
        add_error(ERROR_INVALID_OPERAND, current_filename, -1, "Too many external symbol uses");
    }
}

/*
 * Set the address of an existing entry symbol.
 * The function searches the entries array for the symbol by name.
 * If found, it updates the symbol's address.
 * If the symbol is not found, an error is printed.
*/
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

/* Free extern_uses array
 * Reset all counters
 * We don't need to free entries and externs arrays as they are statically allocated */
void free_entry_extern_resources(void) {
    if (extern_uses != NULL) {
        free(extern_uses);
        extern_uses = NULL;
    }
    entry_count = 0;
    extern_count = 0;
    extern_use_count = 0;

}