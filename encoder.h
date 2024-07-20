/* encoder.h
 *
 * This header file declares the structures and functions used in the encoder.c file
 * for the assembler's first pass. It provides the interface for encoding instructions
 * and managing the first pass of the assembly process.
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>

/* Define constants */
#define NUM_OPCODES 16
#define MAX_LINE_LENGTH 80
#define STARTING_ADDRESS 100
#define RAM_SIZE 4096
#define MAX_SYMBOLS 1000

/* Define types */
typedef unsigned short Word;

/* Enumeration for addressing methods */
typedef enum {
    ADDR_IMMEDIATE,
    ADDR_DIRECT,
    ADDR_INDEX,
    ADDR_REGISTER
} AddressingMethod;

/* Structure for instruction representation */
typedef struct {
    unsigned int opcode : 4;
    unsigned int src_method : 4;
    unsigned int dst_method : 4;
    unsigned int are : 3;
} Instruction;

/* Structure for opcode information */
typedef struct {
    const char* name;
    int value;
} OpcodeInfo;

/* External declarations */
extern const OpcodeInfo opcodes[NUM_OPCODES];
extern Word memory[RAM_SIZE];
extern int IC;

/* Function prototypes */

/* Main function for the first pass encoding process.
 * Reads the input file and encodes all valid instructions.
 *
 * Parameters:
 *   filename: The name of the input assembly file
 */
void encode_first_pass(const char* filename);

#endif /* ENCODER_H */