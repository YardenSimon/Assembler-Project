#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"

#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 256

extern int IC;
extern int DC;
extern MachineWord* memory;
extern int memory_size;


static void update_symbol_addresses(void);
static void create_output_files(const char* filename);
static void write_object_file(const char* filename);
static void write_externals_file(const char* filename);
static void write_entries_file(const char* filename);
static unsigned short create_address_word(int address, int are);
static char* convert_to_octal(unsigned short num);
static char* get_filename_without_extension(const char* filename);


/* Main function for the second pass of the assembler
    It updates all symbol addresses in the assembled code and generates output files.
    */
void perform_second_pass(const char* filename) {

    update_symbol_addresses();
    create_output_files(filename);
}

/* Update symbol addresses in the assembled code
    It creates a new word for each symbol address, containing only the address value and ARE field.
    */
static void update_symbol_addresses(void) {

    int i;
    unsigned short word;
    Symbol* symbol;

    for (i = 0; i < IC + DC - 100; i++) {
        word = memory[i];

        if (word & 0x0002) { /* R bit is set */
            symbol = get_symbol_by_address(i + 100);
            if (symbol != NULL) {
                /* Create a new word with only address and ARE field */
                memory[i] = create_address_word(symbol->address, 2); /* 2 for relocatable */
            }
        } else if (word & 0x0001) { /* E bit is set */
            symbol = get_symbol_by_address(i + 100);
            if (symbol != NULL && symbol->is_external) {
                /* For external symbols, address is 0 and E bit is set */
                memory[i] = create_address_word(0, 1); /* 1 for external */
            }
        }
    }
}


/* This function creates the necessary output files for the assembler.
    1. It always generates the object file (.ob).
    2. It only creates .ext and .ent files if there are external or entry symbols, respectively.
    3. All output files use the input filename as their base.
    */
static void create_output_files(const char* filename) {

    char* base_filename = get_filename_without_extension(filename);

    write_object_file(base_filename);

    if (has_external_symbols()) {
        write_externals_file(base_filename);
    }

    if (has_entry_symbols()) {
        write_entries_file(base_filename);
    }

    free(base_filename);
}

/* Write the object file (.ob) with the assembled machine code
     File format:
     1. First line: <IC> <DC>
     2. In every line: <address in decimal> <word in octal>
     */
static void write_object_file(const char* filename) {

    char ob_filename[MAX_FILENAME_LENGTH];
    FILE* ob_file;
    int i;

    sprintf(ob_filename, "%s.ob", filename);
    ob_file = fopen(ob_filename, "w");
    if (ob_file == NULL) {
        fprintf(stderr, "Error: Unable to create object file.\n");
        return;
    }

    fprintf(ob_file, "%d %d\n", IC - 100, DC);

    for (i = 0; i < IC + DC - 100; i++) {
        fprintf(ob_file, "%04d %s\n", i + 100, convert_to_octal(memory[i]));
    }

    fclose(ob_file);
}

/* Write the externals file (.ext) listing external symbol references
     File format: <symbol_name> <address>
     */
static void write_externals_file(const char* filename) {

    char ext_filename[MAX_FILENAME_LENGTH];
    FILE* ext_file;
    int i;
    Symbol* symbol;

    sprintf(ext_filename, "%s.ext", filename);
    ext_file = fopen(ext_filename, "w");
    if (ext_file == NULL) {
        fprintf(stderr, "Error: Unable to create externals file.\n");
        return;
    }

    for (i = 0; i < IC + DC - 100; i++) {
        if (memory[i] & 0x0001) { /* E bit is set */
            symbol = get_symbol_by_address(i + 100);
            if (symbol != NULL && symbol->is_external) {
                fprintf(ext_file, "%s %04d\n", symbol->name, i + 100);
            }
        }
    }

    fclose(ext_file);
}

/* Write the entries file (.ent) listing entry symbols
    File format: <symbol_name> <address>
    It writes all symbols marked as entry in the symbol table.
    */
static void write_entries_file(const char* filename) {

    char ent_filename[MAX_FILENAME_LENGTH];
    FILE* ent_file;
    Symbol* symbol;

    sprintf(ent_filename, "%s.ent", filename);
    ent_file = fopen(ent_filename, "w");
    if (ent_file == NULL) {
        fprintf(stderr, "Error: Unable to create entries file.\n");
        return;
    }

    symbol = get_first_symbol();
    while (symbol != NULL) {
        if (symbol->is_entry) {
            fprintf(ent_file, "%s %04d\n", symbol->name, symbol->address);
        }
        symbol = get_next_symbol();
    }

    fclose(ent_file);
}

/* Create an address word with only the address value and ARE field
    The address is set to bits 2-16, and the ARE field is set to bits 0-1.
    */
static unsigned short create_address_word(int address, int are) {

    return (address << 2) | (are & 0x03);
}

/* Convert a decimal number to its octal representation as a string
     The octal string is always 5 digits long, with leading zeros if needed.
     */
static char* convert_to_octal(unsigned short num) {

    static char octal_str[7]; /* 5 octal digits + possible negative sign + null terminator */
    sprintf(octal_str, "%05o", num);
    return octal_str;
}

/* This function extracts the filename without its extension */
static char* get_filename_without_extension(const char* filename) {

    char* dot_position = strrchr(filename, '.');
    size_t length = (dot_position != NULL) ? (size_t)(dot_position - filename) : strlen(filename);
    char* result = (char*)malloc(length + 1);

    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for filename.\n");
        exit(1);
    }

    strncpy(result, filename, length);
    result[length] = '\0';

    return result;
}