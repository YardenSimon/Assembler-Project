#include "operand_validation.h"
#include "opcode_groups.h"
#include <ctype.h>
#include <string.h>

int find_command(const char* line) {
    int j;
    int cmdLen;

    /* Skip spaces and labels */
    while (*line && isspace((unsigned char)*line)) line++;
    if (is_label(line)) {
        line = strchr(line, ':') + 1;
        while (*line && isspace((unsigned char)*line)) line++;
    }

    /* Check opcode groups */
    for (j = 0; j < TWO_OPERAND_COUNT; j++) {
        cmdLen = strlen(two_operand_opcodes[j]);
        if (strncmp(line, two_operand_opcodes[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0]));
        }
    }

    for (j = 0; j < ONE_OPERAND_COUNT; j++) {
        cmdLen = strlen(one_operand_opcodes[j]);
        if (strncmp(line, one_operand_opcodes[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0]));
        }
    }

    for (j = 0; j < ZERO_OPERAND_COUNT; j++) {
        cmdLen = strlen(zero_operand_opcodes[j]);
        if (strncmp(line, zero_operand_opcodes[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0]));
        }
    }

    return -1; /* No valid command found */
}

int is_label(const char* str) {
    int i = 0;

    if (!isalpha((unsigned char)str[i])) return 0;

    while (str[++i] != '\0' && str[i] != ':') {
        if (!isalnum((unsigned char)str[i])) return 0;
    }

    return (str[i] == ':' && i > 1);
}

int validate_operand(const char* operand, int command_group) {
    /* Implement operand validation logic here */
    return 1; /* Placeholder return */
}

int count_operands(const char* line) {
    return (strchr(line, ',') != NULL) ? 2 : (strchr(line, ' ') != NULL ? 1 : 0);
}

void extract_operands(const char* line, char* first_operand, char* second_operand) {
    const char* start;
    const char* end;
    size_t length;

    start = strchr(line, ' ');
    if (start == NULL) return;
    start++;

    end = strchr(start, ',');
    if (end == NULL) {
        strcpy(first_operand, start);
        return;
    }

    length = end - start;
    strncpy(first_operand, start, length);
    first_operand[length] = '\0';

    start = end + 1;
    while (*start && isspace((unsigned char)*start)) start++;

    strcpy(second_operand, start);
}