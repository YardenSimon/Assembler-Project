#ifndef ENTRY_EXTERN_H
#define ENTRY_EXTERN_H

#define MAX_ENTRIES 100
#define MAX_EXTERNS 100
#define MAX_EXTERN_USES 1000

#include "globals.h"

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
} EntrySymbol;

typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
} ExternSymbol;

/* For second pass - counts all the appearances of external operands and adds it to a linked list so we eill be
 * able to create the .ext file.
 */
typedef struct {
    char name[MAX_SYMBOL_LENGTH];
    int address;
} ExternUse;

extern EntrySymbol entries[MAX_ENTRIES];
extern ExternSymbol externs[MAX_EXTERNS];
extern int entry_count;
extern int extern_count;
extern ExternUse *extern_uses;
extern int extern_use_count;

/*
 * Add a new entry symbol to the entries array.
 * The function checks if there is space for more symbols.
 * If space is available, it copies the symbol name into the array
 * and sets the address to -1, indicating that it has not been assigned yet.
 */
void add_entry(const char* name);

/*
 * Add a new extern symbol to the externs array.
 * The function checks if there is space for more symbols.
 * If space is available, it copies the symbol name into the array.
 */
void add_extern(const char* name);

/*
 * Initialize entry and extern symbol counts to zero.
 * This function is called at the start of the program
 * to ensure that the symbol tables are empty before use.
 */
void init_entry_extern(void);

/*
 * Initialize memory to save all uses of externals and counts them.
 * This function is called at the start of the seconds pass
 * so at the end of it we can create the .ext file.
 */
void init_extern_uses(void);

/*
 * Set the address of an existing entry symbol.
 * The function searches the entries array for the symbol by name.
 * If found, it updates the symbol's address.
 * If the symbol is not found, an error is printed.
*/
void set_entry_address(const char* name, int address);

/* Adds an occurrence of an extern to the exter uses array*/
void add_extern_use(const char* name, int address);

/* Free extern_uses array
 * Reset all counters
 * We don't need to free entries and externs arrays as they are statically allocated */
void free_entry_extern_resources(void);


#endif