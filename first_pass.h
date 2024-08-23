#ifndef FIRST_PASS_H
#define FIRST_PASS_H

/*
 * Performs the first pass over the assembly file.
 * This function reads each line of the given file, processes it, and prepares memory for instructions and data.
 * It initializes the symbol table and tracks instructions and directives.
 */
void perform_first_pass(const char* filename);

/*
 * Frees the memory used for storing data and resets related variables.
 * This function releases the memory allocated for storing machine words,
 * sets the memory pointer to NULL, and resets memory size, instruction count IC,
 * and DC to their initial values.
 */
void free_memory(void);

/* Adds a new data section.
 * This function records a new section of data memory, defined by start and end addresses.
 * If the maximum number of sections is reached, it sends an error.
 */
void add_data_section(int start_address, int end_address);

#endif