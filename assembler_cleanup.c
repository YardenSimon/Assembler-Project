/* assembler_cleanup.c */

#include "assembler_cleanup.h"
#include "symbol_table.h"
#include <stdlib.h>

extern int IC;
extern int DC;
extern MachineWord* memory;

void cleanup_assembler(void) {
    /* Free symbol table */
    free_symbol_table();

    /* Free memory array */
    if (memory != NULL) {
        free(memory);
        memory = NULL;
    }

    /* Reset counters */
    IC = 0;
    DC = 0;

    /* Add any other cleanup necessary for data structures used in both passes */
}