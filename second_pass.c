/* second_pass.c */

#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"
#include <stdio.h>
#include <string.h>

/* Perform the second pass of the assembler
 * This function completes symbol addressing and instruction encoding */
void perform_second_pass(const char* filename) {
    FILE* file;
    char line[MAX_LINE_LENGTH];
    int address = 100;  /* Start at the same address as the first pass */

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  /* Remove newline */
        if (line[0] == ';' || line[0] == '\0') continue;  /* Skip comments and empty lines */

        if (!is_label(line) && !is_directive(line)) {
            update_symbol_addresses(line);
            finalize_instruction_encoding(&memory[address - 100], line);
            address++;
        }
    }

    fclose(file);

    create_entry_file(filename);
    create_external_file(filename);
    create_object_file(filename);
}

/* Update symbol addresses in a line of assembly code */
void update_symbol_addresses(char* line) {
    /* Implementation to update symbol addresses in the line */
}

/* Finalize the encoding of an instruction */
void finalize_instruction_encoding(MachineWord* instruction, char* line) {
    /* Implementation to finalize the encoding of the instruction */
}

/* Create the entry symbols file */
void create_entry_file(const char* input_filename) {
    char output_filename[MAX_FILENAME_LENGTH];
    FILE* ent_file;

    snprintf(output_filename, sizeof(output_filename), "%s.ent", input_filename);
    ent_file = fopen(output_filename, "w");
    if (ent_file == NULL) {
        fprintf(stderr, "Error creating entry file\n");
        return;
    }
    /* Write entry symbols and their addresses to the file */
    fclose(ent_file);
}

/* Create the external symbols file */
void create_external_file(const char* input_filename) {
    char output_filename[MAX_FILENAME_LENGTH];
    FILE* ext_file;

    snprintf(output_filename, sizeof(output_filename), "%s.ext", input_filename);
    ext_file = fopen(output_filename, "w");
    if (ext_file == NULL) {
        fprintf(stderr, "Error creating external symbols file\n");
        return;
    }
    /* Write external symbols and their usage addresses to the file */
    fclose(ext_file);
}

/* Create the object file containing the final machine code */
void create_object_file(const char* input_filename) {
    char output_filename[MAX_FILENAME_LENGTH];
    FILE* ob_file;

    snprintf(output_filename, sizeof(output_filename), "%s.ob", input_filename);
    ob_file = fopen(output_filename, "w");
    if (ob_file == NULL) {
        fprintf(stderr, "Error creating object file\n");
        return;
    }
    /* Write the final machine code to the file */
    fclose(ob_file);
}