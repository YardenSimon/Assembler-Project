#ifndef OPCODE_GROUPS_H
#define OPCODE_GROUPS_H

/* Two operand opcodes */
#define TWO_OPERAND_COUNT 5
static const char* two_operand_opcodes[TWO_OPERAND_COUNT] = {"mov", "cmp", "add", "sub", "lea"};

/* One operand opcodes */
#define ONE_OPERAND_COUNT 9
static const char* one_operand_opcodes[ONE_OPERAND_COUNT] = {"clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};

/* Zero operand opcodes */
#define ZERO_OPERAND_COUNT 2
static const char* zero_operand_opcodes[ZERO_OPERAND_COUNT] = {"rts", "stop"};

#endif /* OPCODE_GROUPS_H */