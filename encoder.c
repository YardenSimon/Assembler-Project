#include "encoder.h"

int getOpcodeValue(const char* opcode_str) {
    int i;

    /* Loop through the opcodes to find a match */
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(opcode_str, opcodes[i].name) == 0) {
            return opcodes[i].value;
        }
    }

    return -1; /* Invalid opcode */
}

AddressingMethod getAddressingMethod(const char* operand) {
    /* Determine the addressing method based on the operand format */
    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    } else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') {
        return ADDR_REGISTER;
    } else {
        return ADDR_DIRECT;  /* For now, assume it's a label if not immediate or register */
    }
}

Word encodeInstructionWord(const Instruction* inst) {
    Word encodedInst = 0;

    /* Encode the instruction word with opcode and addressing methods */
    encodedInst |= (inst->opcode & 0xF) << 11;
    encodedInst |= ((inst->src_method & 0x1) << 7) |
                   ((inst->src_method & 0x2) << 7) |
                   ((inst->src_method & 0x4) << 7) |
                   ((inst->src_method & 0x8) << 7);
    encodedInst |= ((inst->dst_method & 0x1) << 3) |
                   ((inst->dst_method & 0x2) << 3) |
                   ((inst->dst_method & 0x4) << 3) |
                   ((inst->dst_method & 0x8) << 3);
    encodedInst |= inst->are & 0x7;

    return encodedInst;
}

void encodeInstruction(const char* line) {
    Instruction inst;
    char opcode[5], src[31], dst[31];
    Word encodedInst;
    int value;

    /* Initialize the instruction structure */
    inst.opcode = 0;
    inst.src_method = 0;
    inst.dst_method = 0;
    inst.are = 0;

    /* Parse the line to extract opcode, source, and destination operands */
    sscanf(line, "%4s %30[^,], %30s", opcode, src, dst);

    inst.opcode = getOpcodeValue(opcode);
    inst.src_method = getAddressingMethod(src);
    inst.dst_method = getAddressingMethod(dst);
    inst.are = 0; /* To be implemented later */

    encodedInst = encodeInstructionWord(&inst);
    memory[IC - STARTING_ADDRESS] = encodedInst;
    IC++;

    /* Handle source operand encoding */
    if (inst.src_method == ADDR_IMMEDIATE) {
        value = atoi(src + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    } else if (inst.src_method == ADDR_DIRECT) {
        IC++;
    }

    /* Handle destination operand encoding */
    if (inst.dst_method == ADDR_IMMEDIATE) {
        value = atoi(dst + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    } else if (inst.dst_method == ADDR_DIRECT) {
        IC++;
    }
}
