#ifndef OPERAND_VALIDATION_H
#define OPERAND_VALIDATION_H

#include "globals.h"

typedef struct {
    unsigned char source_allowed;  /* Bitfield for allowed source addressing methods */
    unsigned char dest_allowed;    /* Bitfield for allowed destination addressing methods */
} OpcodeAddressing;

extern const OpcodeAddressing opcode_addressing[NUM_OPCODES];

OpCode find_command(const char* line);
int is_label(const char* str);
int validate_operand(const char* op, OpCode opcode, int is_source);
int count_operands(const char* line);
void extract_operands(const char* line, char* opcode, char* first_operand, char* second_operand);
int validate_instruction(const char* line);



#endif