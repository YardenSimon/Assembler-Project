#include "encoder.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_OPCODES 16
#define MAX_OPERAND_LENGTH 20

extern int IC;  /* Declare IC (Instruction Counter) as extern */
extern MachineWord memory[]; /* Declare memory array as extern, used to store encoded instructions */

/* Enum to represent different addressing methods for operands */
typedef enum {
    ADDR_IMMEDIATE,  /* Immediate value, e.g., #5 */
    ADDR_DIRECT,     /* Direct address or label */
    ADDR_INDEX,      /* Index addressing, e.g., *r3 */
    ADDR_REGISTER    /* Register addressing, e.g., r7 */
} AddressingMethod;

/* Structure to hold information about each opcode */
typedef struct {
    const char* name;
    int value;
} OpcodeInfo;

/* Array of all supported opcodes and their corresponding values */
const OpcodeInfo opcodes[NUM_OPCODES] = {
        {"mov", 0}, {"cmp", 1}, {"add", 2}, {"sub", 3},
        {"lea", 4}, {"clr", 5}, {"not", 6}, {"inc", 7},
        {"dec", 8}, {"jmp", 9}, {"bne", 10}, {"red", 11},
        {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stop", 15}
};

/* Function prototypes for helper functions */
static int get_opcode_value(const char* opcode_name);
static AddressingMethod get_addressing_method(const char* operand);
static void encode_operand(AddressingMethod method, const char* operand);

/* Main function to encode a single instruction
 * This function parses the instruction, identifies its components,
 * and encodes them into machine code */
void encode_instruction(const char* instruction) {
    char opcode_name[5];
    char source[MAX_OPERAND_LENGTH], destination[MAX_OPERAND_LENGTH];
    int opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;

    /* Parse instruction into opcode and operands */
    sscanf(instruction, "%s %[^,], %s", opcode_name, source, destination);

    opcode_value = get_opcode_value(opcode_name);
    src_method = get_addressing_method(source);
    dst_method = get_addressing_method(destination);

    /* Encode first word of instruction
     * This includes the opcode and addressing methods for both operands */
    encoded_word |= (opcode_value & 0xF) << 11;
    encoded_word |= (src_method & 0xF) << 7;
    encoded_word |= (dst_method & 0xF) << 3;
    /* ARE bits set to 0 for now, will be updated in second pass if needed */

    memory[IC - 100] = encoded_word;
    IC++;

    /* Encode operands, which may require additional words */
    encode_operand(src_method, source);
    encode_operand(dst_method, destination);
}

/* Function to get the numeric value of an opcode
 * It searches through the opcodes array to find a match */
static int get_opcode_value(const char* opcode_name) {
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(opcodes[i].name, opcode_name) == 0) {
            return opcodes[i].value;
        }
    }
    return -1; /* Invalid opcode */
}

/* Function to determine the addressing method of an operand
 * It examines the format of the operand string */
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

/* Function to encode an individual operand
 * This function handles the encoding specifics for each addressing method */
static void encode_operand(AddressingMethod method, const char* operand) {
    MachineWord encoded_operand = 0;

    switch (method) {
        case ADDR_IMMEDIATE:
            /* For immediate addressing, convert the value to binary */
            encoded_operand = (MachineWord)(atoi(operand + 1) & 0x7FFF);  /* +1 to skip '#' */
            memory[IC - 100] = encoded_operand;
            IC++;
            break;
        case ADDR_DIRECT:
            /* For direct addressing, leave a placeholder for the address
             * This will be filled in during the second pass */
            IC++;
            break;
        case ADDR_INDEX:
        case ADDR_REGISTER:
            /* For index and register addressing, encode the register number */
            encoded_operand = (MachineWord)((operand[method == ADDR_INDEX ? 2 : 1] - '0') & 0x7);
            if (method == ADDR_INDEX) {
                encoded_operand <<= 3;  /* Shift for index addressing */
            }
            memory[IC - 100] = encoded_operand;
            IC++;
            break;
    }
}