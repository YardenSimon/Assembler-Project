#ifndef OPERAND_VALIDATION_H
#define OPERAND_VALIDATION_H
#define MAX_LABEL_LENGTH 31

int find_command(const char* line);
int is_label(const char* str);
int validate_operand(const char* op, int command_group);
int count_operands(const char* line);
void extract_operands(const char* line, char* first_operand, char* second_operand);
int extract_operands(const char* line, char* first_operand, char* second_operand, int command_group);

#endif