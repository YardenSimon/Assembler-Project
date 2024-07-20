
#include "assembler.h"

/* Define the opcodes and their values */
const OpcodeInfo opcodes[NUM_OPCODES] = {
        {"mov", 0},
        {"cmp", 1},
        {"add", 2},
        {"sub", 3},
        {"lea", 4},
        {"clr", 5},
        {"not", 6},
        {"inc", 7},
        {"dec", 8},
        {"jmp", 9},
        {"bne", 10},
        {"red", 11},
        {"prn", 12},
        {"jsr", 13},
        {"rts", 14},
        {"stop", 15}
};

/* Declare the memory and symbol table */
Word memory[RAM_SIZE];
Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;
int IC = 0, DC = 0;

void init_assembler(void)
{
    int i;
    /* Set initial values for global variables */
    IC = STARTING_ADDRESS;
    DC = 0;
    symbol_count = 0;

    /* Initialize memory to 0 */
    for (i = 0; i < RAM_SIZE; i++)
    {
        memory[i] = 0;
    }
}

int find_opcode(const char* mnemonic)
{
    int i;
    for (i = 0; i < NUM_OPCODES; i++)
    {
        if (strcmp(opcodes[i].name, mnemonic) == 0)
        {
            return opcodes[i].value;
        }
    }
    return -1; /* Opcode not found */
}

void add_to_memory(Word value)
{
    if (IC < RAM_SIZE)
    {
        memory[IC] = value;
        IC++;
    }
    else
    {
        /* Handle memory overflow error */
        printf("Error: Memory overflow\n");
    }
}