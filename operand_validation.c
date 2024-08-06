#include "operand_validation.h"
#include "opcode_groups.h"
#include "encoder.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*In this representation:
  15 (1111 in binary) means all addressing methods are allowed
  14 (1110 in binary) means methods 1, 2, and 3 are allowed, but 0 is not
  6 (0110 in binary) means only methods 1 and 2 are allowed
  2 (0010 in binary) means only method 1 is allowed
  0 means no addressing methods are allowed (null operand)
 */
const OpcodeAddressing opcode_addressing[NUM_OPCODES] = {
        {15, 14}, /* mov  - source: 0,1,2,3; dest: 1,2,3 */
        {15, 15}, /* cmp  - source: 0,1,2,3; dest: 0,1,2,3 */
        {15, 14}, /* add  - source: 0,1,2,3; dest: 1,2,3 */
        {15, 14}, /* sub  - source: 0,1,2,3; dest: 1,2,3 */
        {2,  14}, /* lea  - source: 1;       dest: 1,2,3 */
        {0,  14}, /* clr  - source: null;    dest: 1,2,3 */
        {0,  14}, /* not  - source: null;    dest: 1,2,3 */
        {0,  14}, /* inc  - source: null;    dest: 1,2,3 */
        {0,  14}, /* dec  - source: null;    dest: 1,2,3 */
        {0,   6}, /* jmp  - source: null;    dest: 1,2 */
        {0,   6}, /* bne  - source: null;    dest: 1,2 */
        {0,  14}, /* red  - source: null;    dest: 1,2,3 */
        {0,  15}, /* prn  - source: null;    dest: 0,1,2,3 */
        {0,   6}, /* jsr  - source: null;    dest: 1,2 */
        {0,   0}, /* rts  - source: null;    dest: null */
        {0,   0}  /* stop - source: null;    dest: null */
};

/*The function checks if the opcode is valid.
  It selects the allowed addressing methods for either the source or destination operand.
  It checks if the given method is allowed by using a bitwise AND operation.
 */
int validate_operand_addressing(int opcode, int is_source, int method) {
    unsigned char allowed;

    if (opcode < 0 || opcode >= NUM_OPCODES) {
        return 0;  /* Invalid opcode */
    }

    if (is_source) {
        allowed = opcode_addressing[opcode].source_allowed;
    } else {
        allowed = opcode_addressing[opcode].dest_allowed;
    }

    return (allowed & (1 << method)) != 0;
}


/* find_command purpose is identify the opcode it gets and -
   1. skip whitespace
   2. skip labels (if the line starts with a label)
   3. It checks the no. of operands and locate it to the right group of operands and looking for a match(opcode).
   If it can't find a match, it'll return -1.
 */
int find_command(const char* line) {
    int j;
    int length;
    const char* original_line = line;

    while (*line && isspace((unsigned char)*line)) line++;
    if (is_label(line)) {
        line = strchr(line, ':') + 1;
        while (*line && isspace((unsigned char)*line)) line++;
    }

    for (j = 0; j < TWO_OPERAND_COUNT; j++) {
        length = strlen(two_operand_opcodes[j]);
        if (strncmp(line, two_operand_opcodes[j], length) == 0 &&
            (line[length] == '\0' || isspace((unsigned char)line[length]))) {
            return j;
        }
    }

    for (j = 0; j < ONE_OPERAND_COUNT; j++) {
        length = strlen(one_operand_opcodes[j]);
        if (strncmp(line, one_operand_opcodes[j], length) == 0 &&
            (line[length] == '\0' || isspace((unsigned char)line[length]))) {
            return j + TWO_OPERAND_COUNT;
        }
    }

    for (j = 0; j < ZERO_OPERAND_COUNT; j++) {
        length = strlen(zero_operand_opcodes[j]);
        if (strncmp(line, zero_operand_opcodes[j], length) == 0 &&
            (line[length] == '\0' || isspace((unsigned char)line[length]))) {
            return j + TWO_OPERAND_COUNT + ONE_OPERAND_COUNT;
        }
    }

    return -1;
}


/* is_label function checks if part of the string is label, and if it's a correct label:
   1. It checks if the first charachter is an alpabetic letter using isalpha function.
   2. it checks that all subsequent characters are either alphabetic letters or numbers using isalnum() func.
   3. It checks if the label ends with ':'.
   4. Its validating that the maximum length of the label is 31 charachters.
 */

int is_label(const char* str) {
    int i = 0;
    int label_length = 0;

    while (isspace((unsigned char)str[i])) i++;

    if (!str[i] || !isalpha((int)str[i])) return 0;

    while (str[i] && str[i] != ':') {
        if (!isalnum((int)str[i])) return 0;
        i++;
        label_length++;
        if (label_length > MAX_LABEL_LENGTH) return 0;
    }

    return (str[i] == ':' && label_length > 0 && label_length <= MAX_LABEL_LENGTH);
}

/* This function checks if a given string is a valid label when used as an operand in an instruction.
 * 1. It checks if the first char is a letter.
 * 2. It checks if each character of the label if it's alphanumeric
 * 3. Counts the length of the label and then checks if it's length is less or equal to 31.
 */
int is_valid_label_operand(const char* str) {
    int i = 0;
    int label_length = 0;

    if (!isalpha((int)str[i])) return 0;

    while (str[i]) {
        if (!isalnum((int)str[i])) return 0;
        i++;
        label_length++;
        if (label_length > MAX_LABEL_LENGTH) return 0;
    }

    return (label_length > 0 && label_length <= MAX_LABEL_LENGTH);
}

/* The function checks if the operand is a register or an immediate value (#number) */
int validate_operand(const char* op, int opcode, int is_source) {
    AddressingMethod method;

    if (op == NULL || op[0] == '\0') {
        return opcode_addressing[opcode].source_allowed == 0 && is_source;
    }

    method = get_addressing_method(op);
    if (method == -1) {
        return 0;
    }

    return validate_operand_addressing(opcode, is_source, method);
}

/* This function counts the number of operands in a line of assembly code.*/
int count_operands(const char* line) {
    if (strchr(line, ',') != NULL) {
        return 2;
    } else if (strchr(line, ' ') != NULL) {
        return 1;
    } else {
        return 0;
    }
}

/* The function extracts and validates operands from a line of assembly code.
   1. It assumes the line has no labels and starts with the command.
   2. It looks for the first space in the line, which marks the end of the command and the beginning of the operands.
   3. If no space is found, the function returns 0, indicating no operands.
   4. It then looks for a comma, which would separate two operands.
   5. The function uses validate_operand to check the validity of each extracted operand.
*/
int extract_operands(const char* line, char* first_operand, char* second_operand, int opcode) {
    const char* start;
    const char* end;
    size_t length;

    start = strchr(line, ' ');
    if (start == NULL) return 0;
    start++;

    end = strchr(start, ',');
    if (end == NULL) {
        strcpy(first_operand, start);
        if (!validate_operand(first_operand, opcode, 1)) {
            return -1;
        }
        return 1;
    }

    length = end - start;
    strncpy(first_operand, start, length);
    first_operand[length] = '\0';
    if (!validate_operand(first_operand, opcode, 1)) {
        return -1;
    }

    start = end + 1;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    strcpy(second_operand, start);
    if (!validate_operand(second_operand, opcode, 0)) {
        return -1;
    }

    return 2;
}

int validate_instruction(const char* line) {
    char first_operand[MAX_OPERANDS] = {0};
    char second_operand[MAX_OPERANDS] = {0};
    int opcode = find_command(line);
    int operand_count;

    if (opcode == -1) {
        return 0;
    }

    operand_count = extract_operands(line, first_operand, second_operand, opcode);

    if (operand_count == -1) {
        return 0;
    }

    if (operand_count != count_operands(line)) {
        return 0;
    }

    return 1;
}