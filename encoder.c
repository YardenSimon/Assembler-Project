
#include "encoder.h"

int getOpcodeValue(const char* opcode_str) {
    for (int i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(opcode_str, opcodes[i].name) == 0) {
            return opcodes[i].value;
        }
    }
    return -1; // Invalid opcode
}

AddressingMethod getAddressingMethod(const char* operand) {
    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    } else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') {
        return ADDR_REGISTER;
    } else {
        return ADDR_DIRECT;  // For now, assume it's a label if not immediate or register
    }
}

Word encodeInstructionWord(const Instruction* inst) {
    Word encodedInst = 0;

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
    Instruction inst = {0};
    char opcode[5], src[31], dst[31];
    sscanf(line, "%4s %30[^,], %30s", opcode, src, dst);

    inst.opcode = getOpcodeValue(opcode);
    inst.src_method = getAddressingMethod(src);
    inst.dst_method = getAddressingMethod(dst);
    inst.are = 0; // To be implemented later

    Word encodedInst = encodeInstructionWord(&inst);
    memory[IC - STARTING_ADDRESS] = encodedInst;
    IC++;

    if (inst.src_method == ADDR_IMMEDIATE) {
        int value = atoi(src + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    } else if (inst.src_method == ADDR_DIRECT) {
        IC++;
    }

    if (inst.dst_method == ADDR_IMMEDIATE) {
        int value = atoi(dst + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    } else if (inst.dst_method == ADDR_DIRECT) {
        IC++;
    }
}