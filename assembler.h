#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WORD_SIZE 15
#define RAM_SIZE 4096
#define STARTING_ADDRESS 100
#define MAX_SYMBOLS 1000
#define MAX_LINE_LENGTH 81

typedef unsigned short Word;

typedef enum {
    ADDR_IMMEDIATE = 0,
    ADDR_DIRECT = 1,
    ADDR_INDEX = 2,
    ADDR_REGISTER = 3
} AddressingMethod;

typedef struct {
    unsigned int are : 3;     // A,R,E field (bits 0-2)
    unsigned int dst_method : 4;  // Destination addressing method (bits 3-6)
    unsigned int src_method : 4;  // Source addressing method (bits 7-10)
    unsigned int opcode : 4;  // Opcode (bits 11-14)
} Instruction;

typedef struct {
    char name[31];
    int address;
} Symbol;

typedef struct {
    const char* name;
    int value;
} OpcodeInfo;

#define NUM_OPCODES 16

extern const OpcodeInfo opcodes[NUM_OPCODES];
extern Word memory[RAM_SIZE];
extern Symbol symbolTable[MAX_SYMBOLS];
extern int symbolCount;
extern int IC, DC;

#endif // ASSEMBLER_H