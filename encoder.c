/* encoder.c
 *
 * This file implements the instruction encoding process for the assembler's first pass.
 * It converts valid assembly language instructions into their initial machine code
 * representations, reserving space for addresses to be resolved in the second pass.
 *
 * Key features:
 * 1. Assumes input is pre-validated
 * 2. Handles four addressing methods: Immediate, Direct, Index (*rX), and Register
 * 3. Encodes instructions and reserves space for additional words
 * 4. Provides a single function (encode_first_pass) for the entire first pass encoding
 *
 * The encoder supports the ARE (Absolute, Relocatable, External) encoding system
 * and prepares the groundwork for the second pass address resolution.
 */

#include "encoder.h"
#include <stdlib.h>
#include <string.h>

/* Retrieves the numeric value associated with a given opcode.
 *
 * This function iterates through the predefined opcode table to find a match
 * for the given opcode string. If found, it returns the corresponding numeric value.
 * With pre-validated input, it should always find a match.
 *
 * Parameters:
 *   opcode_str: A string representing the opcode
 *
 * Returns:
 *   The numeric value of the opcode, or -1 if not found (which shouldn't occur
 *   with validated input)
 */
static int get_opcode_value(const char* opcode_str) {
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(opcode_str, opcodes[i].name) == 0) {
            return opcodes[i].value;
        }
    }
    return -1; /* This should never happen with validated input */
}

/* Determines the addressing method of an operand.
 *
 * This function analyzes the format of the operand string to categorize it
 * into one of the four supported addressing methods:
 * - Immediate: Starts with '#'
 * - Index: Starts with '*r' followed by a digit
 * - Register: Starts with 'r' followed by a digit
 * - Direct: Any other format (assumed to be a label or address)
 *
 * Parameters:
 *   operand: A string representing the operand
 *
 * Returns:
 *   The determined AddressingMethod enum value
 */
static AddressingMethod get_addressing_method(const char* operand) {
    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    } else if (operand[0] == '*' && operand[1] == 'r' &&
               operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') {
        return ADDR_INDEX;
    } else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') {
        return ADDR_REGISTER;
    } else {
        return ADDR_DIRECT;
    }
}

/* Encodes the main instruction word.
 *
 * This function combines the opcode, source and destination addressing methods,
 * and the ARE field into a single 15-bit word according to the specified format.
 *
 * Parameters:
 *   inst: Pointer to an Instruction struct containing the instruction details
 *
 * Returns:
 *   The encoded instruction word as a Word (15-bit unsigned integer)
 */
static Word encode_instruction_word(const Instruction* inst) {
    Word encoded_inst = 0;
    encoded_inst |= (inst->opcode & 0xF) << 11;
    encoded_inst |= (inst->src_method & 0xF) << 7;
    encoded_inst |= (inst->dst_method & 0xF) << 3;
    encoded_inst |= inst->are & 0x7;
    return encoded_inst;
}

/* Checks if a symbol is declared as external.
 *
 * This function should be implemented to check if a given symbol
 * is defined as external. The actual implementation depends on how
 * external symbols are tracked in your assembler.
 *
 * Parameters:
 *   symbol: The symbol to check
 *
 * Returns:
 *   1 if the symbol is external, 0 otherwise
 */
static int is_external_symbol(const char* symbol) {
    /* This function should be implemented based on how external symbols are tracked */
    /* For now, it always returns false */
    return 0;
}

/* Encodes a single instruction and reserves space for additional words.
 *
 * This function parses an instruction line, encodes the main instruction word,
 * and reserves space for additional words that will be filled in the second pass.
 * It handles different addressing methods and updates the Instruction Counter (IC)
 * accordingly.
 *
 * Parameters:
 *   line: A string containing the instruction to encode
 */
static void encode_instruction(const char* line) {
    Instruction inst = {0};
    char opcode[5], src[31], dst[31];
    Word encoded_inst;

    sscanf(line, "%4s %30[^,], %30s", opcode, src, dst);
    inst.opcode = get_opcode_value(opcode);
    inst.src_method = get_addressing_method(src);
    inst.dst_method = get_addressing_method(dst);

    encoded_inst = encode_instruction_word(&inst);
    memory[IC - STARTING_ADDRESS] = encoded_inst;
    IC++;

    /* Reserve space for additional words */
    if (inst.src_method == ADDR_IMMEDIATE || inst.src_method == ADDR_DIRECT) {
        IC++;
    }
    if (inst.dst_method == ADDR_IMMEDIATE || inst.dst_method == ADDR_DIRECT) {
        IC++;
    }
    if ((inst.src_method == ADDR_INDEX || inst.src_method == ADDR_REGISTER) &&
        (inst.dst_method == ADDR_INDEX || inst.dst_method == ADDR_REGISTER)) {
        IC++;
    }
}

/* Main function for the first pass encoding process.
 *
 * This function reads the input file, processes each line, and encodes
 * instructions while reserving space for addresses to be resolved in the second pass.
 * It skips comments and empty lines, and handles labels by moving past them
 * to the actual instruction.
 *
 * Parameters:
 *   filename: The name of the input assembly file
 */
void encode_first_pass(const char* filename) {
    FILE* file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    IC = STARTING_ADDRESS;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; /* Remove newline */

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == ';') {
            continue;
        }

        /* Handle labels (assuming labels are already processed) */
        char* instruction = strchr(line, ':');
        if (instruction) {
            instruction++; /* Move past the colon */
            while (*instruction == ' ' || *instruction == '\t') instruction++; /* Skip whitespace */
        } else {
            instruction = line;
        }

        /* Encode the instruction */
        encode_instruction(instruction);
    }

    fclose(file);
}