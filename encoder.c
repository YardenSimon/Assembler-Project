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

/* Array of all opcodes */
const OpcodeInfo opcodes[NUM_OPCODES] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"lea", lea}, {"clr", clr}, {"not", not}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"stop", stop}
};


/*static OpCode get_opcode_value(const char *opcode_name); UNUSED FUNC!!!!!!!!!!!!!*/
static void encode_operand(AddressingMethod method, const char *operand, int is_source);
static void encode_register_operands(const char *source, const char *destination);
static void print_binary(MachineWord word);

/*
 * This function encodes a single assembly instruction into machine code.
 * It performs the following steps:
 * Extracts operands from the instruction string
 * Determines the addressing methods for source and destination operands
 * Validates the operands
 * Encodes the opcode and addressing methods into a machine word
 * Stores the encoded instruction in memory
 * Encodes the operands separately
 * The function uses global variables IC and DC to keep track of
 * instruction and data counters, respectively.
 */
void encode_instruction(const char *instruction, OpCode command_name) {
    char opcode_name[MAX_OPERAND_LENGTH];
    char source[MAX_OPERAND_LENGTH] = {0}, destination[MAX_OPERAND_LENGTH] = {0};
    OpCode opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;

    printf("DEBUG: Encoding instruction: '%s'\n", instruction);/* DELETE!!!!!!!!!!!!!!!!!! */

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

    if (!validate_operand(source, opcode_value, src_method) ||
        (destination[0] != '\0' && !validate_operand(destination, opcode_value, 0))) {
        fprintf(stderr, "Error: Invalid operand(s) in instruction: %s\n", instruction);
        return;
    }

    printf("DEBUG: Opcode: %s, Source method: %d, Destination method: %d\n",
           opcodes[opcode_value].name, src_method, dst_method);/* DELETE!!!!!!!!!!!!!!!!!! */

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

    /* Encode register operands if the source and the destination are registers or index operands */
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

/*
 * This function encodes register operands when both source and destination
 * are either registers or index operands. It extracts the register numbers,
 * encodes them into a single machine word, and stores it in memory.
 * The function handles both direct register addressing
 * and index addressing. It encodes the register numbers
 * into specific bit positions in the machine word and sets the ARE bits.
 */
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

/*
 * This function encodes assembly directives (.data and .string) into machine code.
 * It handles two types of directives:
 * .data: Encodes a list of integer values
 * .string: Encodes a string as ASCII values, including a null terminator
 * The function parses the operands, converts them to appropriate machine words,
 * and stores them in memory. It also performs error checking for invalid input
 * and memory overflow conditions.
 */
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
        operands++;
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

/*
 * This function determines the addressing method of an operand based on its format.
 * It recognizes the following addressing methods:
 * - Immediate: Starts with '#'
 * - Index: Starts with '*'
 * - Register: Starts with 'r' followed by a digit (0-7)
 * - Direct: Any other valid operand
 * Returns:
 *   The determined AddressingMethod enum value
 */
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

/*
 * This function encodes an individual operand based on its addressing method.
 * It handles different addressing methods:
 * - Immediate: Encodes the numeric value
 * - Direct: Stores the operand in a string table and encodes its index
 * - Index/Register: Encodes the register number
 * The function encodes the operand into a machine word and stores it in memory.
 * It also handles error conditions such as string table overflow.
 */
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

/* DEBUG - DELETE! */
static void print_binary(MachineWord word) {
    int i;
    for (i = WORD_SIZE - 1; i >= 0; i--) {
        /* 15 bits, from MSB to LSB */
        printf("%d", (word >> i) & 1);
        if (i % 5 == 0 && i != 0) printf(" "); /* Space every 5 bits for readability */
    }
}

/* This function sets the ARE bits of a machine word.
 * The ARE bits are used to indicate the type of addressing for the word in memory.
 * The function first clears the existing ARE bits in the word, then sets them
 * according to the provided AREType value. This ensures that the ARE bits are
 * always set correctly.
 */
void set_ARE(MachineWord *word, AREType are) {
    *word &= ~((1 << ARE_BITS) - 1); /* Clear the last ARE_BITS bits */
    *word |= are; /* Set the ARE bits */
}