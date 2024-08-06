/* encoder.h */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define WORD_SIZE 15

/* 16-bit unsigned integer type to represent a machine word
 * Used to store 15-bit words, leaving the MSB unused */
typedef unsigned short MachineWord;

/* Encode a single instruction into machine code */
void encode_instruction(const char* instruction);
AddressingMethod get_addressing_method(const char* operand)

#endif /* ENCODER_H */