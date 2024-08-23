#ifndef OPERAND_VALIDATION_H
#define OPERAND_VALIDATION_H

#include "globals.h"

typedef struct {
    unsigned char source_allowed;
    unsigned char dest_allowed;
} OpcodeAddressing;

extern const OpcodeAddressing opcode_addressing[NUM_OPCODES];

/* Looks for a command in a line of code
 * It checks if the line starts with a known command.
 * Returns the OpCode if found, or -1 if not found
 */
OpCode find_command(const char* line);

/* Checks if a string is a valid label.
 * A label should start with a letter, end with a colon,
 * contain only letters and numbers.
 * Returns 1 if it's a valid label, 0 if not
 */
int is_label(const char* str);

/* Checks if an operand is valid for a given command.
 * It makes sure the operand uses the right addressing method for the command.
 * Returns 1 if the operand is valid, 0 if not
 */
int validate_operand(const char* op, OpCode opcode, int is_source);

/* Splits a line of code into its parts
 * It separates the command and up to two operands
 * Puts each part into separate strings
 */
void extract_operands(const char* line, char* opcode, char* first_operand, char* second_operand);

#endif
