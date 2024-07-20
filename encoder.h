#ifndef ENCODER_H
#define ENCODER_H

#include "assembler.h"

/* Function prototypes */
int get_opcode_value(const char* opcode_str);
AddressingMethod get_addressing_method(const char* operand);
Word encode_instruction_word(const Instruction* inst);
void encode_instruction(const char* line);

#endif /* ENCODER_H */