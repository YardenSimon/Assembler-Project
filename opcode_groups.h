#ifndef OPCODE_GROUPS_H
#define OPCODE_GROUPS_H

#include "globals.h"

/* Two operand opcodes */
#define TWO_OPERAND_COUNT 5
static const char* two_operand_opcodes[TWO_OPERAND_COUNT] = {
    OPCODE_NAMES[mov], OPCODE_NAMES[cmp], OPCODE_NAMES[add], OPCODE_NAMES[sub], OPCODE_NAMES[lea]
};

/* One operand opcodes */
#define ONE_OPERAND_COUNT 9
static const char* one_operand_opcodes[ONE_OPERAND_COUNT] = {
    OPCODE_NAMES[clr], OPCODE_NAMES[not], OPCODE_NAMES[inc], OPCODE_NAMES[dec],
    OPCODE_NAMES[jmp], OPCODE_NAMES[bne], OPCODE_NAMES[red], OPCODE_NAMES[prn], OPCODE_NAMES[jsr]
};

/* Zero operand opcodes */
#define ZERO_OPERAND_COUNT 2
static const char* zero_operand_opcodes[ZERO_OPERAND_COUNT] = {
    OPCODE_NAMES[rts], OPCODE_NAMES[stop]
};

#endif /* OPCODE_GROUPS_H */