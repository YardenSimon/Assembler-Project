/* encoder.h */


#ifndef ENCODER_H
#define ENCODER_H

#include "globals.h"


/* MachineWord represents a 15-bit word in our assembly language.
 * We use a 16-bit unsigned short, but only utilize the lower 15 bits. */
typedef unsigned short MachineWord;
extern int memory_size;
extern MachineWord *memory;


/* AddressingMethod enum represents the five addressing methods in our assembly language:
 * 0 - None: No operand
 * 1 - Immediate: A constant value, e.g., #5
 * 2 - Direct: A memory address or label
 * 3 - Index: Addressing using a base address and an index register, e.g., *r3
 * 4 - Register: Direct register addressing, e.g., r7 */
typedef enum {
 ADDR_NONE,
 ADDR_IMMEDIATE,
 ADDR_DIRECT,
 ADDR_INDEX,
 ADDR_REGISTER
} AddressingMethod;

/* AREType enum represents the ARE (Absolute, Relocatable, External) bits:
 * ARE_ABSOLUTE (4): The A bit (bit 2) is on, used for constants and instructions
 * ARE_RELOCATABLE (2): The R bit (bit 1) is on, used for relocatable addresses
 * ARE_EXTERNAL (1): The E bit (bit 0) is on, used for external references */


/* Function to encode a single instruction into machine code */
void encode_instruction(const char* instruction, OpCode command_name);
void encode_directive(const char* directive, const char* operands);

/* Function to determine the addressing method of an operand */
AddressingMethod get_addressing_method(const char* operand);

/* Function to set the ARE bits for a word */
void set_ARE(MachineWord* word, AREType are);

#endif /* ENCODER_H */