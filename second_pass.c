/* second_pass.c */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"

#define MAX_FILENAME_LENGTH 256

extern int IC;
extern int DC;
extern MachineWord* memory;

static void update_symbol_addresses(void);
static void create_output_files(const char* filename);
static void write_object_file(const char* filename);
static void write_externals_file(const char* filename);
static void write_entries_file(const char* filename);
static char* convert_to_octal(unsigned short num);
static char* get_filename_without_extension(const char* filename);

void perform_second_pass(const char* filename) {
    printf("DEBUG: Starting second pass for file: %s\n", filename);

    update_symbol_addresses();
    create_output_files(filename);

    printf("DEBUG: Second pass completed successfully\n");
}

static void update_symbol_addresses(void) {
    int i;
    Symbol* symbol;
    char symbol_name[MAX_SYMBOL_LENGTH + 1];

    printf("DEBUG: Updating symbol addresses\n");
    for (i = 0; i < IC + DC - 100; i++) {
        if ((memory[i] & 0x0007) == ARE_RELOCATABLE) {
            strncpy(symbol_name, (char*)&memory[i], sizeof(MachineWord));
            symbol_name[sizeof(MachineWord)] = '\0';
            symbol = get_symbol_by_name(symbol_name);
            if (symbol != NULL) {
                if (symbol->is_external) {
                    memory[i] = (symbol->address << 3) | ARE_EXTERNAL;
                    printf("DEBUG: Updated external symbol %s at address %d\n", symbol_name, i + 100);
                } else {
                    memory[i] = (symbol->address << 3) | ARE_RELOCATABLE;
                    printf("DEBUG: Updated relocatable symbol %s at address %d\n", symbol_name, i + 100);
                }
            } else {
                fprintf(stderr, "Error: Undefined symbol %s at address %d\n", symbol_name, i + 100);
            }
        }
    }
}

static void create_output_files(const char* filename) {
    char* base_filename = get_filename_without_extension(filename);
    write_object_file(base_filename);
    write_externals_file(base_filename);
    write_entries_file(base_filename);
    free(base_filename);
}

static void write_object_file(const char* filename) {
    char ob_filename[MAX_FILENAME_LENGTH];
    FILE* ob_file;
    int i;

    sprintf(ob_filename, "%s.ob", filename);
    ob_file = safe_fopen(ob_filename, "w");

    fprintf(ob_file, "%d %d\n", IC - 100, DC);

    for (i = 0; i < IC + DC - 100; i++) {
        fprintf(ob_file, "%04d %s\n", i + 100, convert_to_octal(memory[i]));
    }

    fclose(ob_file);
    printf("DEBUG: Object file '%s' created successfully\n", ob_filename);
}

static void write_externals_file(const char* filename) {
    char ext_filename[MAX_FILENAME_LENGTH];
    FILE* ext_file = NULL;
    int i;
    Symbol* symbol;
    int external_count = 0;

    /* First, count the number of external symbols */
    for (i = 0; i < get_symbol_count(); i++) {
        symbol = get_symbol_by_index(i);
        if (symbol->is_external) {
            external_count++;
        }
    }

    /* Only create the file if there are external symbols */
    if (external_count > 0) {
        sprintf(ext_filename, "%s.ext", filename);
        ext_file = safe_fopen(ext_filename, "w");

        for (i = 0; i < get_symbol_count(); i++) {
            symbol = get_symbol_by_index(i);
            if (symbol->is_external) {
                fprintf(ext_file, "%s %04d\n", symbol->name, symbol->address);
                printf("DEBUG: Wrote external symbol %s at address %04d\n", symbol->name, symbol->address);
            }
        }

        fclose(ext_file);
        printf("DEBUG: Externals file '%s' created successfully\n", ext_filename);
    } else {
        printf("DEBUG: No external symbols found, externals file not created\n");
    }
}

static void write_entries_file(const char* filename) {
    char ent_filename[MAX_FILENAME_LENGTH];
    FILE* ent_file = NULL;
    int i;
    Symbol* symbol;
    int entry_count = 0;

    /* First, count the number of entry symbols */
    for (i = 0; i < get_symbol_count(); i++) {
        symbol = get_symbol_by_index(i);
        if (symbol->is_entry) {
            entry_count++;
        }
    }

    /* Only create the file if there are entry symbols */
    if (entry_count > 0) {
        sprintf(ent_filename, "%s.ent", filename);
        ent_file = safe_fopen(ent_filename, "w");

        for (i = 0; i < get_symbol_count(); i++) {
            symbol = get_symbol_by_index(i);
            if (symbol->is_entry) {
                fprintf(ent_file, "%s %04d\n", symbol->name, symbol->address);
                printf("DEBUG: Wrote entry symbol %s at address %04d\n", symbol->name, symbol->address);
            }
        }

        fclose(ent_file);
        printf("DEBUG: Entries file '%s' created successfully\n", ent_filename);
    } else {
        printf("DEBUG: No entry symbols found, entries file not created\n");
    }
}

static char* convert_to_octal(unsigned short num) {
    static char octal_str[7];
    int i;
    unsigned short mask = 0x7000;

    for (i = 0; i < 5; i++) {
        octal_str[i] = ((num & mask) >> (12 - 3*i)) + '0';
        mask >>= 3;
    }
    octal_str[5] = '\0';

    return octal_str;
}

static char* get_filename_without_extension(const char* filename) {
    BaseFilename base_filename = get_base_filename(filename);
    char* result = (char*)safe_malloc(base_filename.length + 1);

    strncpy(result, base_filename.name, base_filename.length);
    result[base_filename.length] = '\0';

    free(base_filename.name);
    return result;
}