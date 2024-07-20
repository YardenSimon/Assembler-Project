#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "assembler.h"
#include <stdio.h>

/* Function to add a symbol to the symbol table */
void add_symbol(const char* name, int address);

/* Function to check if a line contains a label */
int is_label(const char* line);

/* Function to extract the label name from a line */
void get_label_name(const char* line, char* label, int max_length);

/* Function to process data directives (.data and .string) */
void process_data_directive(const char* directive);

/* Function to report errors with line numbers */
void report_error(const char* message, int line_number);

/* Main function for the first pass of the assembler */
void first_pass(const char* filename);

#endif /* FIRST_PASS_H */