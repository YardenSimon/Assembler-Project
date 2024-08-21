#include "utils.h"
#include "operand_validation.h"
#include "opcode_groups.h"
#include "encoder.h"
#include "globals.h"
#include "errors.h"
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
    {2, 14}, /* lea  - source: 1;       dest: 1,2,3 */
    {0, 14}, /* clr  - source: null;    dest: 1,2,3 */
    {0, 14}, /* not  - source: null;    dest: 1,2,3 */
    {0, 14}, /* inc  - source: null;    dest: 1,2,3 */
    {0, 14}, /* dec  - source: null;    dest: 1,2,3 */
    {0, 6}, /* jmp  - source: null;    dest: 1,2 */
    {0, 6}, /* bne  - source: null;    dest: 1,2 */
    {0, 14}, /* red  - source: null;    dest: 1,2,3 */
    {0, 15}, /* prn  - source: null;    dest: 0,1,2,3 */
    {0, 6}, /* jsr  - source: null;    dest: 1,2 */
    {0, 0}, /* rts  - source: null;    dest: null */
    {0, 0} /* stop - source: null;    dest: null */
};


/*
 * Finds and returns the opcode corresponding to the command found in a given line.
 */
OpCode find_command(const char *line) {
    int j;
    int length;
    char *current = (char *) line;

    printf("DEBUG: Finding command in line: '%s'\n", line);

    skip_whitespace(&current);

    /* Check for number(0-2) of operands opcodes */

    for (j = 0; j < TWO_OPERAND_COUNT; j++) {
        length = strlen(two_operand_opcodes[j]);
        if (strncmp(current, two_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char) current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j);
            return (OpCode) j;
        }
    }

    for (j = 0; j < ONE_OPERAND_COUNT; j++) {
        length = strlen(one_operand_opcodes[j]);
        if (strncmp(current, one_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char) current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j + TWO_OPERAND_COUNT);
            return (OpCode) (j + TWO_OPERAND_COUNT);
        }
    }

    for (j = 0; j < ZERO_OPERAND_COUNT; j++) {
        length = strlen(zero_operand_opcodes[j]);
        if (strncmp(current, zero_operand_opcodes[j], length) == 0 &&
            (current[length] == '\0' || isspace((unsigned char) current[length]))) {
            printf("DEBUG: Found opcode: %d\n", j + TWO_OPERAND_COUNT + ONE_OPERAND_COUNT);
            return (OpCode) (j + TWO_OPERAND_COUNT + ONE_OPERAND_COUNT);
        }
    }

    printf("DEBUG: Invalid opcode\n");
    return (OpCode) -1;
}

/*
 * Checks if the given string is a valid label.
 * A label must start with an alphabetical character and end with a colon ':'.
 */
int is_label(const char *str) {
    int i = 0;
    int label_length = 0;

    printf("DEBUG: Checking if '%s' is a label\n", str);
    while (isspace((unsigned char) str[i])) i++;
    if (!str[i] || !isalpha((int) str[i])) return 0;
    while (str[i] && str[i] != ':') {
        if (!isalnum((int) str[i])) return 0;
        i++;
        label_length++;
        if (label_length > MAX_LABEL_LENGTH) return 0;
    }

    printf("DEBUG: Label check result: %d\n", (str[i] == ':' && label_length > 0));
    return (str[i] == ':' && label_length > 0);
}


/* Validates whether a given operand is valid for the specified opcode.*/
int validate_operand(const char *op, OpCode opcode, int is_source) {
    AddressingMethod method;

    printf("DEBUG: Validating operand: '%s', Opcode: %s, Is source: %d\n",
           op, OPCODE_NAMES[opcode], is_source);

    if (op == NULL || op[0] == '\0') {
        return (is_source && opcode_addressing[opcode].source_allowed == 0) ||
               (!is_source && opcode_addressing[opcode].dest_allowed == 0);
    }

    method = get_addressing_method(op);
    if (method == -1) {
        printf("DEBUG: Invalid addressing method\n");
        add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Invalid addressing method for operand: %s", op);
        return 0;
    }

    /* For opcodes with only one operand, always treat it as a destination operand */
    if (opcode_addressing[opcode].source_allowed == 0 && opcode_addressing[opcode].dest_allowed != 0) {
        is_source = 0;
    }

    /* Check if the addressing method is allowed for this operand */
    if (is_source) {
        if (!(opcode_addressing[opcode].source_allowed & (1 << (method - 1)))) {
            printf("DEBUG: Invalid source addressing method for opcode\n");
            add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Invalid source addressing method for opcode: %s", OPCODE_NAMES[opcode]);
            return 0;
        }
    } else {
        if (!(opcode_addressing[opcode].dest_allowed & (1 << (method - 1)))) {
            printf("DEBUG: Invalid destination addressing method for opcode\n");
            add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Invalid destination addressing method for opcode: %s", OPCODE_NAMES[opcode]);
            return 0;
        }
    }

    printf("DEBUG: Operand validation result: 1\n");
    return 1;
}


/* Extracts the opcode and operands from a given line of text. */
void extract_operands(const char *line, char *opcode, char *first_operand, char *second_operand) {
    const char *start = line;
    const char *end;
    size_t length;

    printf("DEBUG: Extracting operands from line: '%s'\n", line);

    opcode[0] = first_operand[0] = second_operand[0] = '\0';
    while (*start && isspace((unsigned char) *start)) start++;

    end = start;
    while (*end && !isspace((unsigned char) *end)) end++;
    length = end - start;
    if (length > 0 && opcode != NULL) {
        strncpy(opcode, start, length);
        opcode[length] = '\0';
    } else {
        printf("DEBUG: No opcode found\n");
        add_error(ERROR_INVALID_INSTRUCTION, current_filename, current_line_number, "No opcode found in instruction");
        return;
    }

    /* Move to the first operand */
    start = end;
    while (*start && isspace((unsigned char) *start)) start++;

    /* Extract first operand if exist */
    if (*start) {
        end = start;
        while (*end && *end != ',' && !isspace((unsigned char) *end)) end++;
        length = end - start;
        strncpy(first_operand, start, length);
        first_operand[length] = '\0';

        /* Move to the second operand */
        start = end;
        while (*start && (*start == ',' || isspace((unsigned char) *start))) start++;

        /* Extract second operand if exist */
        if (*start) {
            end = start + strlen(start);
            while (end > start && isspace((unsigned char) *(end - 1))) end--;
            length = end - start;
            strncpy(second_operand, start, length);
            second_operand[length] = '\0';
        }
    }

    printf("DEBUG: Extracted - Opcode: '%s', First operand: '%s', Second operand: '%s'\n",
           opcode, first_operand, second_operand);
}


