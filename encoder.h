#ifndef ENCODER_H
#define ENCODER_H

#include "assembler.h"

/* Function prototypes */
int getOpcodeValue(const char* opcode_str);
AddressingMethod getAddressingMethod(const char* operand);
Word encodeInstructionWord(const Instruction* inst);
void encodeInstruction(const char* line);

#endif /* ENCODER_H */
