/* encoder.c */

#include "encoder.h"
#include "operand_validation.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_OPERAND_LENGTH 20

extern int IC;
extern int DC;

/* Structure to hold information about each opcode */
typedef struct {
    const char *name;
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
static OpCode get_opcode_value(const char *opcode_name);

static void encode_operand(AddressingMethod method, const char *operand, int is_source);

static void encode_register_operands(const char *source, const char *destination);

static void print_binary(MachineWord word);

/* Encode a single instruction into machine code */
void encode_instruction(const char *instruction, OpCode command_name) {
    char opcode_name[MAX_OPERAND_LENGTH];
    char source[MAX_OPERAND_LENGTH] = {0}, destination[MAX_OPERAND_LENGTH] = {0};
    OpCode opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;

    printf("DEBUG: Encoding instruction: '%s'\n", instruction);

    extract_operands(instruction, opcode_name, source, destination);

    opcode_value = command_name;
    src_method = get_addressing_method(source);
    dst_method = get_addressing_method(destination);

    if (dst_method == ADDR_NONE) {
        if (src_method != ADDR_NONE) {
            dst_method = src_method;
            src_method = ADDR_NONE;
        }
    }
    /* Add validation here */
    if (!validate_operand(source, opcode_value, src_method) ||
        (destination[0] != '\0' && !validate_operand(destination, opcode_value, 0))) {
        fprintf(stderr, "Error: Invalid operand(s) in instruction: %s\n", instruction);
        return;
    }

    printf("DEBUG: Opcode: %s, Source method: %d, Destination method: %d\n",
           opcodes[opcode_value].name, src_method, dst_method);

    encoded_word |= ((opcode_value & 0xF) << 11);

    /* Encode source addressing method */
    switch (src_method) {
        case ADDR_IMMEDIATE:
            encoded_word |= (1 << 7);
            break;
        case ADDR_DIRECT:
            encoded_word |= (1 << 8);
            break;
        case ADDR_INDEX:
            encoded_word |= (1 << 9);
            break;
        case ADDR_REGISTER:
            encoded_word |= (1 << 10);
            break;
        default:
            break;
    }

    /* Encode destination addressing method */
    switch (dst_method) {
        case ADDR_IMMEDIATE:
            encoded_word |= (1 << 3);
            break;
        case ADDR_DIRECT:
            encoded_word |= (1 << 4);
            break;
        case ADDR_INDEX:
            encoded_word |= (1 << 5);
            break;
        case ADDR_REGISTER:
            encoded_word |= (1 << 6);
            break;
        default:
            break;
    }

    encoded_word |= ARE_ABSOLUTE;

    memory[IC + DC - 100] = encoded_word;
    IC++;

    printf("DEBUG: Encoded instruction word: ");
    print_binary(encoded_word);
    printf("\n");

    if ((src_method == ADDR_REGISTER || src_method == ADDR_INDEX) &&
        (dst_method == ADDR_REGISTER || dst_method == ADDR_INDEX)) {
        encode_register_operands(source, destination);
    } else {
        if (src_method != ADDR_NONE) {
            encode_operand(src_method, source, 1);
        }
        if (dst_method != ADDR_NONE) {
            if (src_method == ADDR_NONE) {
                encode_operand(dst_method, source, 0);
            } else { encode_operand(dst_method, destination, 0); }
        }
    }
}

static void encode_register_operands(const char *source, const char *destination) {
    MachineWord encoded_word = 0;
    int src_reg, dst_reg;

    /* Extract source register number, handling both 'r3' and '*r3' formats */
    src_reg = (source[0] == '*') ? source[2] - '0' : source[1] - '0';

    /* Extract destination register number, handling both 'r3' and '*r3' formats */
    dst_reg = (destination[0] == '*') ? destination[2] - '0' : destination[1] - '0';

    encoded_word |= (src_reg & 0x7) << 6;
    encoded_word |= (dst_reg & 0x7) << 3;
    encoded_word |= ARE_ABSOLUTE;

    memory[IC + DC - 100] = encoded_word;
    IC++;

    printf("DEBUG: Encoded register operands: ");
    print_binary(encoded_word);
    printf("\n");
}

void encode_directive(const char *directive, const char *operands) {
    char *endptr;
    long value;

    printf("DEBUG: Encoding directive: '%s' with operands: '%s'\n", directive, operands);

    if (strcmp(directive, ".data") == 0) {
        while (*operands) {
            while (isspace((unsigned char) *operands) || *operands == ',') operands++;
            if (*operands == '\0') break;

            value = strtol(operands, &endptr, 10);
            if (operands == endptr) {
                fprintf(stderr, "Error: Invalid number in .data directive\n");
                return;
            }
            if (DC >= MEMORY_SIZE) {
                fprintf(stderr, "Error: Data segment overflow\n");
                return;
            }
            /* Encode the value into 15 bits */
            memory[IC + DC - 100] = (MachineWord) (value & 0x7FFF);
            printf("DEBUG: Encoded .data value: ");
            print_binary((MachineWord) (value & 0x7FFF));
            printf("\n");
            DC++;
            operands = endptr;
        }
    } else if (strcmp(directive, ".string") == 0) {
        if (*operands != '"') {
            fprintf(stderr, "Error: String must start with a quote\n");
            return;
        }
        operands++; /* Skip opening quote */
        while (*operands && *operands != '"') {
            if (DC >= MEMORY_SIZE) {
                fprintf(stderr, "Error: Data segment overflow\n");
                return;
            }
            /* Encode ASCII value of the character */
            memory[IC + DC - 100] = (MachineWord) (*operands & 0x7F);
            printf("DEBUG: Encoded character: '%c' as ", *operands);
            print_binary((MachineWord) (*operands & 0x7F));
            printf("\n");
            DC++;
            operands++;
        }
        if (*operands != '"') {
            fprintf(stderr, "Error: String must end with a quote\n");
            return;
        }
        /* Add null terminator */
        if (DC >= MEMORY_SIZE) {
            fprintf(stderr, "Error: Data segment overflow\n");
            return;
        }
        memory[IC + DC - 100] = 0;
        printf("DEBUG: Encoded null terminator: ");
        print_binary(0);
        printf("\n");
        DC++;
    } else {
        fprintf(stderr, "Error: Unknown directive %s\n", directive);
    }
}

/* Determine the addressing method of an operand */
AddressingMethod get_addressing_method(const char *operand) {
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
static void encode_operand(AddressingMethod method, const char *operand, int is_source) {
    MachineWord encoded_operand = 0;
    int register_num;

    printf("DEBUG: Encoding operand: '%s', Method: %d, Is source: %d\n",
           operand, method, is_source);

    switch (method) {
        case ADDR_IMMEDIATE:
            encoded_operand = ((MachineWord) safe_atoi(operand + 1) & 0xFFF) << 3;
            encoded_operand |= ARE_ABSOLUTE;
            break;
        case ADDR_DIRECT:
            /* Store index to string table */
            if (string_count < MAX_STRINGS) {
                strncpy(string_table[string_count], operand, MAX_STRING_LENGTH - 1);
                string_table[string_count][MAX_STRING_LENGTH - 1] = '\0';
                encoded_operand = (string_count << 2) | ARE_RELOCATABLE;
                string_count++;
            } else {
                fprintf(stderr, "Error: String table full\n");
                return;
            }
            break;
        case ADDR_INDEX:
        case ADDR_REGISTER:
            register_num = (operand[0] == '*') ? operand[2] - '0' : operand[1] - '0';
            if (is_source) {
                encoded_operand |= (register_num & 0x7) << 6;
            } else {
                encoded_operand |= (register_num & 0x7) << 3;
            }
            encoded_operand |= ARE_ABSOLUTE;
            break;
        default:
            fprintf(stderr, "Error: Unknown addressing method\n");
            return;
    }

    memory[IC + DC - INITIAL_MEMORY_ADDRESS] = encoded_operand;
    IC++;

    printf("DEBUG: Encoded operand word: ");
    if (method == ADDR_DIRECT) {
        printf("%s (index: %d)\n", operand, encoded_operand >> 2);
    } else {
        print_binary(encoded_operand);
        printf("\n");
    }
}

/* Print a MachineWord in binary format */
static void print_binary(MachineWord word) {
    int i;
    for (i = WORD_SIZE - 1; i >= 0; i--) {
        /* 15 bits, from MSB to LSB */
        printf("%d", (word >> i) & 1);
        if (i % 5 == 0 && i != 0) printf(" "); /* Space every 5 bits for readability */
    }
}

/* Set the ARE bits for a word */
void set_ARE(MachineWord *word, AREType are) {
    *word &= ~((1 << ARE_BITS) - 1); /* Clear the last ARE_BITS bits */
    *word |= are; /* Set the ARE bits */
}
