/* encoder.h */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define WORD_SIZE 15

/* 16-bit unsigned integer type to represent a machine word
 * Used to store 15-bit words, leaving the MSB unused */
typedef unsigned short MachineWord;

/* Enum to represent different addressing methods for operands */
typedef enum {
    ADDR_IMMEDIATE,  /* Immediate value, for example #5 */
    ADDR_DIRECT,     /* Direct address or label */
    ADDR_INDEX,      /* Index addressing, for example *r3 */
    ADDR_REGISTER    /* Register addressing, for example r7 */
} AddressingMethod;

/* Encode a single instruction into machine code */
void encode_instruction(const char* instruction);

/* Determine the addressing method of an operand */
AddressingMethod get_addressing_method(const char* operand);

#endif /* ENCODER_H */