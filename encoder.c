#include "encoder.h"
#include <stdlib.h>
#include <string.h>

int get_opcode_value(const char* opcode_str)
{
    int i;
    /* Loop through the opcodes to find a match */
    for (i = 0; i < NUM_OPCODES; i++)
    {
        if (strcmp(opcode_str, opcodes[i].name) == 0)
        {
            return opcodes[i].value;
        }
    }
    return -1; /* Invalid opcode */
}

AddressingMethod get_addressing_method(const char* operand)
{
    /* Determine the addressing method based on the operand format */
    if (operand[0] == '#')
    {
        return ADDR_IMMEDIATE;
    }
    else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0')
    {
        return ADDR_REGISTER;
    }
    else
    {
        return ADDR_DIRECT; /* For now, assume it's a label if not immediate or register */
    }
}

Word encode_instruction_word(const Instruction* inst)
{
    Word encoded_inst;
    /* Encode the instruction word with opcode and addressing methods */
    encoded_inst = 0;
    encoded_inst |= (inst->opcode & 0xF) << 11;
    encoded_inst |= (inst->src_method & 0xF) << 7;
    encoded_inst |= (inst->dst_method & 0xF) << 3;
    encoded_inst |= inst->are & 0x7;
    return encoded_inst;
}

void encode_instruction(const char* line)
{
    Instruction inst;
    char opcode[5], src[31], dst[31];
    Word encoded_inst;
    int value;

    /* Initialize the instruction structure */
    inst.opcode = 0;
    inst.src_method = 0;
    inst.dst_method = 0;
    inst.are = 0;

    /* Parse the line to extract opcode, source, and destination operands */
    sscanf(line, "%4s %30[^,], %30s", opcode, src, dst);

    inst.opcode = get_opcode_value(opcode);
    inst.src_method = get_addressing_method(src);
    inst.dst_method = get_addressing_method(dst);
    inst.are = 0; /* To be implemented later */

    encoded_inst = encode_instruction_word(&inst);
    memory[IC - STARTING_ADDRESS] = encoded_inst;
    IC++;

    /* Handle source operand encoding */
    if (inst.src_method == ADDR_IMMEDIATE)
    {
        value = atoi(src + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    }
    else if (inst.src_method == ADDR_DIRECT)
    {
        IC++;
    }

    /* Handle destination operand encoding */
    if (inst.dst_method == ADDR_IMMEDIATE)
    {
        value = atoi(dst + 1);
        memory[IC - STARTING_ADDRESS] = (value & 0x7FFF) << 3;
        IC++;
    }
    else if (inst.dst_method == ADDR_DIRECT)
    {
        IC++;
    }
}