/* encoder.c */

#include "encoder.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_OPCODES 16
#define MAX_OPERAND_LENGTH 20

extern int IC;
extern int DC;
extern MachineWord* memory;
extern int memory_size;

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
static void encode_operand(AddressingMethod method, const char* operand, int is_source);
static void print_binary(MachineWord word);

/* Encode a single instruction into machine code */
void encode_instruction(const char* instruction) {
    char opcode_name[5];
    char source[MAX_OPERAND_LENGTH], destination[MAX_OPERAND_LENGTH];
    int opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;

    printf("DEBUG: Encoding instruction: %s\n", instruction);

    /* Parse instruction into opcode and operands */
    sscanf(instruction, "%s %[^,], %s", opcode_name, source, destination);

    printf("DEBUG: Parsed - Opcode: %s, Source: %s, Destination: %s\n", opcode_name, source, destination);

    opcode_value = get_opcode_value(opcode_name);
    src_method = get_addressing_method(source);
    dst_method = get_addressing_method(destination);

    printf("DEBUG: Opcode value: %d, Source method: %d, Destination method: %d\n", opcode_value, src_method, dst_method);

    /* Encode first word of instruction */
    encoded_word |= ((opcode_value & 0xF) << 11);
    encoded_word |= (1 << (src_method + 7));  /* Set the correct bit for source addressing method */
    encoded_word |= (1 << (dst_method + 3));  /* Set the correct bit for destination addressing method */
    /* ARE bits set to 0 for now, will be updated in second pass if needed */

    encoded_word &= 0x7FFF;  /* Ensure only 15 bits are used */

    printf("DEBUG: Encoded first word: ");
    print_binary(encoded_word);
    printf("\n");

    if (IC - 100 + DC >= memory_size) {
        memory_size *= 2;
        memory = (MachineWord*)realloc(memory, memory_size * sizeof(MachineWord));
        if (memory == NULL) {
            fprintf(stderr, "Error: Memory reallocation failed\n");
            exit(1);
        }
        printf("DEBUG: Memory resized. New size: %d\n", memory_size);
    }

    memory[IC - 100] = encoded_word;
    printf("DEBUG: Stored encoded word at memory[%d]\n", IC - 100);
    IC++;

    /* Encode operands, which may require additional words */
    encode_operand(src_method, source, 1);  /* 1 indicates source operand */
    encode_operand(dst_method, destination, 0);  /* 0 indicates destination operand */

    printf("DEBUG: Finished encoding instruction. New IC: %d\n", IC);
}

/* Get the numeric value of an opcode */
static int get_opcode_value(const char* opcode_name) {
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(opcodes[i].name, opcode_name) == 0) {
            printf("DEBUG: Found opcode %s with value %d\n", opcode_name, opcodes[i].value);
            return opcodes[i].value;
        }
    }
    printf("DEBUG: Invalid opcode: %s\n", opcode_name);
    return -1; /* Invalid opcode */
}

/* Determine the addressing method of an operand */
AddressingMethod get_addressing_method(const char* operand) {
    AddressingMethod method;
    if (operand[0] == '#') {
        method = ADDR_IMMEDIATE;
    } else if (operand[0] == '*' && operand[1] == 'r' &&
               operand[2] >= '0' && operand[2] <= '7' && operand[3] == '\0') {
        method = ADDR_INDEX;
    } else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') {
        method = ADDR_REGISTER;
    } else {
        method = ADDR_DIRECT;
    }
    printf("DEBUG: Addressing method for operand %s: %d\n", operand, method);
    return method;
}

/* Encode an individual operand */
static void encode_operand(AddressingMethod method, const char* operand, int is_source) {
    MachineWord encoded_operand = 0;
    int register_num;

    printf("DEBUG: Encoding operand: %s (method: %d, is_source: %d)\n", operand, method, is_source);

    switch (method) {
        case ADDR_IMMEDIATE:
            encoded_operand = (MachineWord)atoi(operand + 1);  /* +1 to skip '#' */
            encoded_operand &= 0x7FFF;  /* Ensure only 15 bits are used */
            memory[IC - 100] = encoded_operand;
            printf("DEBUG: Encoded immediate value: ");
            print_binary(encoded_operand);
            printf(" at memory[%d]\n", IC - 100);
            IC++;
            break;
        case ADDR_DIRECT:
            /* Placeholder for address, to be filled in second pass */
            printf("DEBUG: Direct addressing placeholder at memory[%d]\n", IC - 100);
            IC++;
            break;
        case ADDR_INDEX:
        case ADDR_REGISTER:
            register_num = operand[method == ADDR_INDEX ? 2 : 1] - '0';
            if (is_source) {
                encoded_operand |= (register_num & 0x7) << 6;  /* Bits 6-8 for source */
            } else {
                encoded_operand |= (register_num & 0x7) << 3;  /* Bits 3-5 for destination */
            }
            encoded_operand &= 0x7FFF;  /* Ensure only 15 bits are used */
            memory[IC - 100] = encoded_operand;
            printf("DEBUG: Encoded register/index: ");
            print_binary(encoded_operand);
            printf(" at memory[%d]\n", IC - 100);
            IC++;
            break;
    }
    printf("DEBUG: Finished encoding operand. New IC: %d\n", IC);
}

/* Print a MachineWord in binary format */
static void print_binary(MachineWord word) {
    int i;
    for (i = 14; i >= 0; i--) {  /* 15 bits, from MSB to LSB */
        printf("%d", (word >> i) & 1);
        if (i % 5 == 0 && i != 0) printf(" ");  /* Space every 5 bits for readability */
    }
}