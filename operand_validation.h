#ifndef OPERAND_VALIDATION_H
#define OPERAND_VALIDATION_H
#define MAX_LABEL_LENGTH 31
#define MAX_OPERANDS 2
#define NUM_OPCODES 16

typedef struct {
    unsigned char source_allowed;  /* Bitfield for allowed source addressing methods */
    unsigned char dest_allowed;    /* Bitfield for allowed destination addressing methods */
} OpcodeAddressing;

extern const OpcodeAddressing opcode_addressing[NUM_OPCODES];

int find_command(const char* line);
int is_label(const char* str);
int validate_operand(const char* op, int opcode, int is_source);
int count_operands(const char* line);
int extract_operands(const char* line, char* first_operand, char* second_operand, int opcode);
int validate_instruction(const char* line);



#endif