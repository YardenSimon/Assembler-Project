#ifndef OPERAND_VALIDATION_H
#define OPERAND_VALIDATION_H

#include <stddef.h>

/* Find and validate a command in a line */
int find_command(const char* line);

/* Check if a string is a valid label */
int is_label(const char* str);

/* Validate an operand */
int validate_operand(const char* operand, int command_group);

/* Count operands in a line */
int count_operands(const char* line);

/* Extract operands from a line */
void extract_operands(const char* line, char* first_operand, char* second_operand);

#endif /* OPERAND_VALIDATION_H */