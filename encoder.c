#include "encoder.h"
#include "operand_validation.h"
#include "utils.h"
#include "errors.h"
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>


extern int IC;
extern int DC;
int memory_address = INITIAL_MEMORY_ADDRESS;

EncodedData encoded_data = {NULL, NULL, 0};

const OpcodeInfo opcodes[NUM_OPCODES] = {
    {"mov", mov}, {"cmp", cmp}, {"add", add}, {"sub", sub},
    {"lea", lea}, {"clr", clr}, {"not", not}, {"inc", inc},
    {"dec", dec}, {"jmp", jmp}, {"bne", bne}, {"red", red},
    {"prn", prn}, {"jsr", jsr}, {"rts", rts}, {"stop", stop}
};


/* Adds an encoded word to the encoded data structure.
   This function creates a new node and adds it to the end of the linked list.
*/
static void add_to_encoded_data(MachineWord word);

/* This function encodes an individual operand based on its addressing method.
 * It handles different addressing methods:
 * - Immediate: Encodes the numeric value
 * - Direct: Stores the operand in a string table and encodes its index
 * - Index/Register: Encodes the register number
 * The function encodes the operand into a machine word and stores it in memory.
 * It also handles error conditions such as string table overflow.
 */
static void encode_operand(AddressingMethod method, const char *operand, int is_source);


/* This function encodes register operands when both source and destination
 * are either registers or index operands. It extracts the register numbers,
 * encodes them into a single machine word, and stores it in memory.
 * The function handles both direct register addressing
 * and index addressing. It encodes the register numbers
 * into specific bit positions in the machine word and sets the ARE bits.
 */
static void encode_register_operands(const char *source, const char *destination);


void init_encoded_data(void) {
    encoded_data.head = NULL;
    encoded_data.tail = NULL;
    encoded_data.count = 0;
}

static void add_to_encoded_data(MachineWord word) {
    EncodedDataNode *new_node = (EncodedDataNode *) safe_malloc(sizeof(EncodedDataNode));
    if (new_node == NULL) {
        add_error(ERROR_MEMORY_ALLOCATION, current_filename, current_line_number,
                  "Unable to allocate memory for encoded data");
        return;
    }
    new_node->word = word;
    new_node->next = NULL;

    if (encoded_data.tail == NULL) {
        encoded_data.head = new_node;
        encoded_data.tail = new_node;
    } else {
        encoded_data.tail->next = new_node;
        encoded_data.tail = new_node;
    }

    encoded_data.count++;
    DC = encoded_data.count;
}


void encode_instruction(const char *instruction, OpCode command_name) {
    char opcode_name[MAX_OPERAND_LENGTH];
    char source[MAX_OPERAND_LENGTH] = {0}, destination[MAX_OPERAND_LENGTH] = {0};
    OpCode opcode_value;
    AddressingMethod src_method, dst_method;
    MachineWord encoded_word = 0;

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
        add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Invalid operand(s) in instruction: %s",
                  instruction);
        return;
    }
    encoded_word |= ((opcode_value & 0xF) << 11);

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
    memory[memory_address - INITIAL_MEMORY_ADDRESS] = encoded_word;
    IC++;
    memory_address++;

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

    src_reg = (source[0] == '*') ? source[2] - '0' : source[1] - '0';
    dst_reg = (destination[0] == '*') ? destination[2] - '0' : destination[1] - '0';

    encoded_word |= (src_reg & 0x7) << 6;
    encoded_word |= (dst_reg & 0x7) << 3;
    encoded_word |= ARE_ABSOLUTE;

    memory[memory_address - INITIAL_MEMORY_ADDRESS] = encoded_word;
    IC++;
    memory_address++;
}


void encode_directive(const char *directive, const char *operands) {
    char *endptr;
    long value;

    if (strcmp(directive, ".data") == 0) {
        while (*operands) {
            while (isspace((unsigned char) *operands) || *operands == ',') operands++;
            if (*operands == '\0') break;

            value = strtol(operands, &endptr, 10);
            if (operands == endptr) {
                add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number,
                          "Invalid number in .data directive");
                return;
            }

           /* Encode the value into 15 bits */
            add_to_encoded_data((MachineWord) (value & 0x7FFF));
            memory_address++;
            operands = endptr;
        }
    } else if (strcmp(directive, ".string") == 0) {
        if (*operands != '"') {
            add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "String must start with a quote");
            return;
        }

        operands++;

        while (*operands && *operands != '"') {
            /* Encode ASCII value of the character */
            add_to_encoded_data((MachineWord) (*operands & 0x7F));
            memory_address++;
            operands++;
        }

        if (*operands != '"') {
            add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "String must end with a quote");
            return;
        }

        add_to_encoded_data(0);
        memory_address++;

    } else {
        add_error(ERROR_INVALID_INSTRUCTION, current_filename, current_line_number, "Unknown directive %s", directive);
    }
}


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


static void encode_operand(AddressingMethod method, const char *operand, int is_source) {
    MachineWord encoded_operand = 0;
    int register_num;

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
                add_error(ERROR_MEMORY_ALLOCATION, current_filename, current_line_number, "String table full");
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
            add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Unknown addressing method");
        return;
    }

    memory[memory_address - INITIAL_MEMORY_ADDRESS] = encoded_operand;
    IC++;
    memory_address++;
}


void add_encoded_data_to_memory(void) {
    EncodedDataNode *current = encoded_data.head;
    int i = 0;

    while (current != NULL) {
        memory[memory_address + i - DC - INITIAL_MEMORY_ADDRESS] = current->word;
        current = current->next;
        i++;
    }
}


void set_ARE(MachineWord *word, AREType are) {
    *word &= ~((1 << ARE_BITS) - 1);
    *word |= are;
}

void free_encoded_data(void) {
    EncodedDataNode *current = encoded_data.head;
    EncodedDataNode *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    encoded_data.head = NULL;
    encoded_data.tail = NULL;
    encoded_data.count = 0;
}

