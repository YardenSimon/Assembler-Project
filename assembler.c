/*
 * Created by yarde on 06/07/2024.
 */
#include "assembler.h"

/* Define the opcodes and their values */
const OpcodeInfo opcodes[NUM_OPCODES] = {
        {"mov", 0}, {"cmp", 1}, {"add", 2}, {"sub", 3},
        {"lea", 4}, {"clr", 5}, {"not", 6}, {"inc", 7},
        {"dec", 8}, {"jmp", 9}, {"bne", 10}, {"red", 11},
        {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stop", 15}
};

/* Declare the memory and symbol table */
Word memory[RAM_SIZE];
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int IC, DC;
