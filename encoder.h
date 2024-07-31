#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define WORD_SIZE 15

/* Define a 16-bit unsigned integer type to represent a machine word
 * We use 16 bits to store our 15-bit words, leaving the most significant bit unused
 * This type ensures we have a consistent 16-bit size across different systems */
typedef unsigned short MachineWord;

/* Function to encode a single instruction
 * This function takes a string representation of an assembly instruction
 * and converts it into its machine code equivalent */
void encode_instruction(const char* instruction);

#endif /* ENCODER_H */