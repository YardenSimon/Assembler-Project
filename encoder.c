/* encoder.c */

#include "encoder.h"

#include "operand_validation.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

#define MAX_OPERAND_LENGTH 20

extern int IC;
extern int DC;
extern MachineWord* memory;
extern int memory_size;

/* Structure to hold information about each opcode */
typedef struct {
    const char* name;
    OpCode value;
} OpcodeInfo;

/* Array of all supported opcodes and their corresponding values */
const OpcodeInfo opcodes[NUM_OPCODES] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"lea", lea}, {"clr", clr}, {"not", not}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"stop", stop}
};



/* Function prototypes for helper functions */
static OpCode get_opcode_value(const char* opcode_name);
static void encode_operand(AddressingMethod method, const char* operand, int is_source);
static void print_binary(MachineWord word);

/* Encode a single instruction into machine code */
void encode_instruction(const char* instruction, OpCode command_name) {
    char opcode_name[MAX_OPERAND_LENGTH];
    char source[MAX_OPERAND_LENGTH] = {0}, destination[MAX_OPERAND_LENGTH] = {0};
    OpCode opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;
    int operand_count;

    printf("DEBUG: Encoding instruction: %s\n", instruction);

    /* Parse instruction into opcode and operands */
    extract_operands(instruction, opcode_name, source, destination);

    printf("DEBUG: Parsed - Opcode: %d, Source: %s, Destination: %s\n", command_name, source, destination);

    opcode_value = command_name;
    src_method = get_addressing_method(source);
    dst_method = get_addressing_method(destination);

    printf("DEBUG: Opcode value: %d, Source method: %d, Destination method: %d\n", opcode_value, src_method, dst_method);
    // if (!validate_instruction(instruction)) {
    //     fprintf(stderr, "Error: Invalid instruction\n");
    //     return;
    // }
    /* Encode first word of instruction */
    encoded_word |= ((opcode_value & ((1 << OPCODE_BITS) - 1)) << (WORD_SIZE - OPCODE_BITS));
    encoded_word |= ((src_method & ((1 << SOURCE_ADDR_BITS) - 1)) << (WORD_SIZE - OPCODE_BITS - SOURCE_ADDR_BITS));
    encoded_word |= ((dst_method & ((1 << DEST_ADDR_BITS) - 1)) << (WORD_SIZE - OPCODE_BITS - SOURCE_ADDR_BITS - DEST_ADDR_BITS));
    set_ARE(&encoded_word, ARE_ABSOLUTE);  /* Set A bit for instruction word */

    printf("DEBUG: Encoded first word: ");
    print_binary(encoded_word);
    printf("\n");

    if (IC - 100 + DC >= memory_size) {
        memory_size *= 2;
        memory = (MachineWord*)safe_malloc(memory_size * sizeof(MachineWord));
        printf("DEBUG: Memory resized. New size: %d\n", memory_size);
    }

    memory[IC - 100] = encoded_word;
    printf("DEBUG: Stored encoded word at memory[%d]\n", IC - 100);
    IC++;

    /* Encode operands, which may require additional words */
    if (src_method != ADDR_NONE) {
        encode_operand(src_method, source, 1);  /* 1 indicates source operand */
    }
    if (dst_method != ADDR_NONE) {
        encode_operand(dst_method, destination, 0);  /* 0 indicates destination operand */
    }

    printf("DEBUG: Finished encoding instruction. New IC: %d\n", IC);
}

/* Determine the addressing method of an operand */
AddressingMethod get_addressing_method(const char* operand) {
    if (operand == NULL || operand[0] == '\0') {
        return ADDR_NONE;
    } else if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    } else if (operand[0] == '*') {
        return ADDR_INDEX;
    } else if (operand[0] == 'r' && operand[1] >= '0' && operand[1] < '0' + NUM_REGISTERS && operand[2] == '\0') {
        return ADDR_REGISTER;
    } else {
        return ADDR_DIRECT;
    }
}

/* Encode an individual operand */
static void encode_operand(AddressingMethod method, const char* operand, int is_source) {
    MachineWord encoded_operand = 0;
    Register register_num;

    printf("DEBUG: Encoding operand: %s (method: %d, is_source: %d)\n", operand, method, is_source);

    switch (method) {
        case ADDR_IMMEDIATE:
            encoded_operand = (MachineWord)safe_atoi(operand + 1);  /* +1 to skip '#' */
        set_ARE(&encoded_operand, ARE_ABSOLUTE);
        break;
        case ADDR_DIRECT:
            /* For direct addressing, store the operand as a string */
                strncpy((char*)&encoded_operand, operand, sizeof(MachineWord));
        set_ARE(&encoded_operand, ARE_RELOCATABLE);  /* Will be resolved in second pass */
        break;
        case ADDR_INDEX:
            register_num = (Register)(operand[2] - '0');
        encoded_operand = (1 << 15) | register_num;  /* Set high bit to mark as index */
        set_ARE(&encoded_operand, ARE_RELOCATABLE);  /* Will be resolved in second pass */
        break;
        case ADDR_REGISTER:
            register_num = (Register)(operand[1] - '0');;
        encoded_operand = register_num;
        set_ARE(&encoded_operand, ARE_ABSOLUTE);
        break;
        default:
            fprintf(stderr, "Error: Unknown addressing method\n");
        return;
    }

    memory[IC - 100] = encoded_operand;
    printf("DEBUG: Encoded operand: ");
    print_binary(encoded_operand);
    printf(" at memory[%d]\n", IC - 100);
    IC++;
}

/* Print a MachineWord in binary format */
static void print_binary(MachineWord word) {
    int i;
    for (i = WORD_SIZE - 1; i >= 0; i--) {  /* 15 bits, from MSB to LSB */
        printf("%d", (word >> i) & 1);
        if (i % 5 == 0 && i != 0) printf(" ");  /* Space every 5 bits for readability */
    }
}
/* Set the ARE bits for a word */
void set_ARE(MachineWord* word, AREType are) {
    *word &= ~((1 << ARE_BITS) - 1);  /* Clear the last ARE_BITS bits */
    *word |= are;     /* Set the ARE bits */
}