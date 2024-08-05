/* first_pass.h */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#define MAX_LINE_LENGTH 80

/* Do the first pass of the assembler
 * This function reads the input file, handles each line,
 * makes the symbol table, and partly codes instructions */
void perform_first_pass(const char* filename);

#endif /* FIRST_PASS_H */