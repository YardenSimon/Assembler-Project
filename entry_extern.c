#include "entry_extern.h"
#include <string.h>
#include <stdio.h>

/* Array of entry symbols */
EntrySymbol entries[MAX_ENTRIES];
int entry_count = 0;
/* Array for extern symbols */
ExternSymbol externs[MAX_EXTERNS];
int extern_count = 0;

/*
 * Initialize entry and extern symbol counts to zero.
 * This function is typically called at the start of the program
 * to ensure that the symbol tables are empty before use.
 */
void init_entry_extern(void) {
    entry_count = 0;
    extern_count = 0;
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
        fprintf(stderr, "Error: Too many entry symbols\n");
    }
}


/*
 * Add a new extern symbol to the externs array.
 * The function checks if there is space for more symbols.
 * If space is available, it copies the symbol name into the array.
 */
void add_extern(const char* name, int address) {
    if (extern_count < MAX_EXTERNS) {
        strncpy(externs[extern_count].name, name, MAX_SYMBOL_LENGTH - 1);
        externs[extern_count].name[MAX_SYMBOL_LENGTH - 1] = '\0';
        entries[entry_count].address = address;
        extern_count++;
    } else {
        fprintf(stderr, "Error: Too many extern symbols\n");
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
    fprintf(stderr, "Error: Entry symbol '%s' not found\n", name);
}