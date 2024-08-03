/* second_pass.h */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 256

/* Perform the second pass of the assembler */
void perform_second_pass(const char* filename);

/* Update symbol addresses in a line of assembly code */
void update_symbol_addresses(char* line);

/* Finalize the encoding of an instruction */
void finalize_instruction_encoding(MachineWord* instruction, char* line);

/* Create the entry symbols file */
void create_entry_file(const char* input_filename);

/* Create the external symbols file */
void create_external_file(const char* input_filename);

/* Create the object file containing the final machine code */
void create_object_file(const char* input_filename);

#endif /* SECOND_PASS_H */