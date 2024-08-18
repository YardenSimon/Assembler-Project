#include "globals.h"
#include "utils.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"
#include "entry_extern.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern int IC;
extern int DC;
extern MachineWord *memory;

static void update_symbol_addresses(void);

static void create_output_files(const char *filename);

static void write_object_file(const char *filename);

static void write_externals_file(const char *filename);

static void write_entries_file(const char *filename);

static char *convert_to_octal(unsigned short num);

static char *get_filename_without_extension(const char *filename);


/* DELETE LATER !!!!!!!!!!!!!!!!!!!!!!!!!*/
void print_externals(void);

void perform_second_pass(const char *filename) {
    printf("DEBUG: Starting second pass for file: %s\n", filename);

    update_symbol_addresses();

    print_externals();

    create_output_files(filename);

    printf("DEBUG: Second pass completed successfully\n");
}

/* In second_pass.c */

static void update_symbol_addresses(void) {
    int i, j;
    Symbol *symbol;
    MachineWord word;
    int string_index;

    printf("DEBUG: Updating symbol addresses\n");

    for (i = 0; i < IC + DC - INITIAL_MEMORY_ADDRESS; i++) {
        word = memory[i];

        if ((word & 3) == ARE_RELOCATABLE) {
            string_index = word >> 2;
            if (string_index < string_count) {
                symbol = get_symbol_by_name(string_table[string_index]);
                if (symbol != NULL) {
                    /*SHIFT 2 LEFT*/
                    MachineWord new_word = ((symbol->address & 0xFFF) << 3) | ARE_RELOCATABLE;
                    printf("DEBUG: Updated memory[%d] for symbol %s\n", i, string_table[string_index]);
                    printf("Old value: %d\n", word);
                    printf("New value: %d\n", new_word);
                    memory[i] = new_word;
                } else {
                    /* If symbol is not found, it might be an external symbol */
                    MachineWord new_word = ARE_EXTERNAL;
                    printf("DEBUG: Symbol %s not found in table, assuming external\n", string_table[string_index]);
                    add_extern(string_table[string_index], i + INITIAL_MEMORY_ADDRESS );
                    printf("DEBUG: Added extern %s in address: %d\n", string_table[string_index], i + INITIAL_MEMORY_ADDRESS);

                    printf("Old value: %d\n", word);
                    printf("New value: %d\n", new_word);
                    memory[i] = new_word;
                }
            } else {
                fprintf(stderr, "Error: Invalid string table index\n");
            }
        }
    }
    for (i = 0; i < entry_count; i++) {
        symbol = get_symbol_by_name(entries[i].name);
        if (symbol != NULL) {
            set_entry_address(entries[i].name, symbol->address);
        } else {
            fprintf(stderr, "Error: Entry symbol '%s' not found in symbol table\n", entries[i].name);
        }
    }
}


static void create_output_files(const char *filename) {
    char *base_filename = get_filename_without_extension(filename);
    write_object_file(base_filename);
    write_externals_file(base_filename);
    write_entries_file(base_filename);
    free(base_filename);
}

static void write_object_file(const char *filename) {
    char ob_filename[MAX_FILENAME_LENGTH];
    FILE *ob_file;
    int i;

    sprintf(ob_filename, "%s%s", filename, OBJECT_FILE_EXT);
    ob_file = safe_fopen(ob_filename, "w");

    fprintf(ob_file, "  %d %d\n", IC - INITIAL_MEMORY_ADDRESS, DC);

    for (i = 0; i < (IC + DC - INITIAL_MEMORY_ADDRESS); i++) {
        fprintf(ob_file, "%04d %s\n", i + INITIAL_MEMORY_ADDRESS, convert_to_octal(memory[i]));
    }

    fclose(ob_file);
    printf("DEBUG: Object file '%s' created successfully\n", ob_filename);
}

static void write_externals_file(const char *filename) {
    char ext_filename[MAX_FILENAME_LENGTH];
    FILE *ext_file = NULL;
    int i;

    if (extern_count > 0) {
        snprintf(ext_filename, sizeof(ext_filename), "%s.ext", filename);
        ext_file = safe_fopen(ext_filename, "w");

        for (i = 0; i < extern_count; i++) {
            fprintf(ext_file, "%s\t%04d\n", externs[i].name, externs[i].address);
            printf("DEBUG: Wrote extern symbol %s at address %04d\n", externs[i].name, externs[i].address);
        }

        fclose(ext_file);
        printf("DEBUG: Externals file '%s' created successfully\n", ext_filename);
    } else {
        printf("DEBUG: No external symbols found, externals file not created\n");
    }
}


static void write_entries_file(const char *filename) {
    char ent_filename[MAX_FILENAME_LENGTH];
    FILE *ent_file = NULL;
    int i;

    if (entry_count > 0) {
        snprintf(ent_filename, sizeof(ent_filename), "%s%s", filename, ENTRIES_FILE_EXT);
        ent_file = safe_fopen(ent_filename, "w");

        for (i = 0; i < entry_count; i++) {
            if (entries[i].address != -1) {
                fprintf(ent_file, "%s %04d\n", entries[i].name, entries[i].address);
                printf("DEBUG: Wrote entry symbol %s at address %04d\n", entries[i].name, entries[i].address);
            } else {
                fprintf(stderr, "Error: Entry symbol '%s' has no address\n", entries[i].name);
            }
        }

        fclose(ent_file);
        printf("DEBUG: Entries file '%s' created successfully\n", ent_filename);
    } else {
        printf("DEBUG: No entry symbols found, entries file not created\n");
    }
}


static char *convert_to_octal(unsigned short num) {
    static char octal_str[7];
    int i;
    unsigned short temp = num;

    for (i = 4; i >= 0; i--) {
        octal_str[i] = (temp & 7) + '0';
        temp >>= 3;
    }
    octal_str[5] = '\0';

    return octal_str;
}

static char *get_filename_without_extension(const char *filename) {
    BaseFilename base_filename = get_base_filename(filename);
    char *result = (char *) safe_malloc(base_filename.length + 1);

    strncpy(result, base_filename.name, base_filename.length);
    result[base_filename.length] = '\0';

    free(base_filename.name);
    return result;
}

/* DELETE LATER !!!!!!!!!!!!!!!!!!!!!!!!!*/
void print_externals(void) {
    int i;
    printf("\n--- External Symbols ---\n");
    printf("%-*s %-10s\n", MAX_SYMBOL_LENGTH, "Name", "Address");
    printf("----------------------------------------\n");
    for (i = 0; i < extern_count; i++) {
        printf("%-*s %-10d\n",
               MAX_SYMBOL_LENGTH,
               externs[i].name,
               externs[i].address);
    }
    printf("----------------------------------------\n");
    printf("Total external symbols: %d\n", extern_count);
}