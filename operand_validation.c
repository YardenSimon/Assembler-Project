#include "utils.h"
#include "operand_validation.h"
#include "opcode_groups.h"
#include "encoder.h"
#include "globals.h"
#include <ctype.h>
#include <stdio.h>
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

int validate_operand_addressing(OpCode opcode, int is_source, int method) {
    unsigned char allowed;

    printf("DEBUG: Validating operand addressing - Opcode: %s, Is source: %d, Method: %d\n",
           OPCODE_NAMES[opcode], is_source, method);

    if (opcode < 0 || opcode >= NUM_OPCODES) {
        return 0;  /* Invalid opcode */
    }

    if (is_source) {
        allowed = opcode_addressing[opcode].source_allowed;
    } else {
        allowed = opcode_addressing[opcode].dest_allowed;
    }

    printf("DEBUG: Operand addressing validation result: %d\n", (allowed & (1 << method)) != 0);
    return (allowed & (1 << method)) != 0;
}

OpCode find_command(const char* line) {
    int j;
    int length;
    char* current = (char*)line;  /* Cast to non-const for skip_whitespace */

    printf("DEBUG: Finding command in line: '%s'\n", line);

    skip_whitespace(&current);

    for (j = 0; j < TWO_OPERAND_COUNT; j++) {
        length = strlen(two_operand_opcodes[j]);
        if (strncmp(current, two_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char)current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j);
            return (OpCode)j;
        }
    }

    for (j = 0; j < ONE_OPERAND_COUNT; j++) {
        length = strlen(one_operand_opcodes[j]);
        if (strncmp(current, one_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char)current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j + TWO_OPERAND_COUNT);
            return (OpCode)(j + TWO_OPERAND_COUNT);
        }
    }

    for (j = 0; j < ZERO_OPERAND_COUNT; j++) {
        length = strlen(zero_operand_opcodes[j]);
        if (strncmp(current, zero_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char)current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j + TWO_OPERAND_COUNT + ONE_OPERAND_COUNT);
            return (OpCode)(j + TWO_OPERAND_COUNT + ONE_OPERAND_COUNT);
        }
    }

    printf("DEBUG: Invalid opcode\n");
    return (OpCode)-1;  /* Invalid opcode */
}

int is_label(const char* str) {
    int i = 0;
    int label_length = 0;

    printf("DEBUG: Checking if '%s' is a label\n", str);

    while (isspace((unsigned char)str[i])) i++;

    if (!str[i] || !isalpha((int)str[i])) return 0;

    while (str[i] && str[i] != ':') {
        if (!isalnum((int)str[i])) return 0;
        i++;
        label_length++;
        if (label_length > MAX_LABEL_LENGTH) return 0;
    }

    printf("DEBUG: Label check result: %d\n", (str[i] == ':' && label_length > 0));
    return (str[i] == ':' && label_length > 0);
}

int is_valid_label_operand(const char* str) {
    int i = 0;
    int label_length = 0;

    printf("DEBUG: Checking if '%s' is a valid label operand\n", str);

    if (!isalpha((int)str[i])) return 0;

    while (str[i]) {
        if (!isalnum((int)str[i])) return 0;
        i++;
        label_length++;
        if (label_length > MAX_LABEL_LENGTH) return 0;
    }

    printf("DEBUG: Valid label operand check result: %d\n", (label_length > 0));
    return (label_length > 0);
}

int validate_operand(const char* op, OpCode opcode, int is_source) {
    AddressingMethod method;

    printf("DEBUG: Validating operand: '%s', Opcode: %s, Is source: %d\n",
           op, OPCODE_NAMES[opcode], is_source);

    if (op == NULL || op[0] == '\0') {
        return opcode_addressing[opcode].source_allowed == 0 && is_source;
    }

    method = get_addressing_method(op);
    if (method == -1) {
        printf("DEBUG: Invalid addressing method\n");
        return 0;
    }

    printf("DEBUG: Operand validation result: %d\n", validate_operand_addressing(opcode, is_source, method));
    return validate_operand_addressing(opcode, is_source, method);
}

int count_operands(const char* line) {
    int count = 0;
    printf("DEBUG: Counting operands in line: '%s'\n", line);

    if (strchr(line, ',') != NULL) {
        count = 2;
    } else if (strchr(line, ' ') != NULL) {
        count = 1;
    }

    printf("DEBUG: Operand count: %d\n", count);
    return count;
}

void extract_operands(const char* line, char* opcode, char* first_operand, char* second_operand) {
    const char* start = line;
    const char* end;
    size_t length;

    printf("DEBUG: Extracting operands from line: '%s'\n", line);

    /* Initialize output parameters */
    opcode[0] = first_operand[0] = second_operand[0] = '\0';

    /* Skip leading whitespace */
    while (*start && isspace((unsigned char)*start)) start++;

    /* Extract opcode */
    end = start;
    while (*end && !isspace((unsigned char)*end)) end++;
    length = end - start;
    if (length > 0 && opcode != NULL) {
        strncpy(opcode, start, length);
        opcode[length] = '\0';
    } else {
        printf("DEBUG: No opcode found\n");
        return; /* No opcode found */
    }

    /* Move to first operand */
    start = end;
    while (*start && isspace((unsigned char)*start)) start++;

    /* Extract first operand if present */
    if (*start) {
        end = start;
        while (*end && *end != ',' && !isspace((unsigned char)*end)) end++;
        length = end - start;
        strncpy(first_operand, start, length);
        first_operand[length] = '\0';

        /* Move to second operand */
        start = end;
        while (*start && (*start == ',' || isspace((unsigned char)*start))) start++;

        /* Extract second operand if present */
        if (*start) {
            end = start + strlen(start);
            while (end > start && isspace((unsigned char)*(end-1))) end--;
            length = end - start;
            strncpy(second_operand, start, length);
            second_operand[length] = '\0';
        }
    }

    printf("DEBUG: Extracted - Opcode: '%s', First operand: '%s', Second operand: '%s'\n",
           opcode, first_operand, second_operand);
}

int validate_instruction(const char* line) {
    char opcode[MAX_OPERANDS] = {0};
    char first_operand[MAX_OPERANDS] = {0};
    char second_operand[MAX_OPERANDS] = {0};
    OpCode opcode_value;
    int operand_count;
    int expected_count;

    printf("DEBUG: Validating instruction: '%s'\n", line);

    opcode_value = find_command(line);
    if (opcode_value == (OpCode)-1) {
        printf("DEBUG: Invalid opcode\n");
        return 0;
    }

    extract_operands(line, opcode, first_operand, second_operand);
    operand_count = (*first_operand != '\0') + (*second_operand != '\0');

    printf("DEBUG: Extracted operands - First: '%s', Second: '%s', Count: %d\n",
           first_operand, second_operand, operand_count);

    expected_count = count_operands(line);
    printf("DEBUG: Expected operand count: %d, Actual count: %d\n", expected_count, operand_count);

    if (operand_count != expected_count) {
        printf("DEBUG: Operand count mismatch\n");
        return 0;
    }

    if (!validate_operand(first_operand, opcode_value, 1) ||
        (operand_count == 2 && !validate_operand(second_operand, opcode_value, 0))) {
        printf("DEBUG: Invalid operand(s)\n");
        return 0;
    }

    printf("DEBUG: Instruction validated successfully\n");
    return 1;
}