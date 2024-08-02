/* first_pass.h */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#define MAX_LINE_LENGTH 80

/* Perform the first pass of the assembler */
void perform_first_pass(const char* filename);

/* Free dynamically allocated memory */
void free_memory();

#endif /* FIRST_PASS_H */