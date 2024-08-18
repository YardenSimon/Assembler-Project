#include "globals.h"
#include "utils.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"
#include "entry_extern.h"
#include "errors.h"
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


/*
 * Performs the second pass of processing on the given filename.
 * This function updates symbol addresses and creates the necessary output files.
 */
void perform_second_pass(const char *filename) {
    int j;
    printf("DEBUG: Starting second pass for file: %s\n", filename);

    init_extern_uses();
    printf("DEBUG: Externals in second pass:\n");
    for (j = 0; j < extern_count; j++) {
        printf("DEBUG: External %d: %s\n", j, externs[j].name);
    }
    update_symbol_addresses();
    create_output_files(filename);
    free_entry_extern_resources();

    printf("DEBUG: Second pass completed successfully\n");
}

/*
 * Updates the addresses of symbols in memory.
 * This function replaces relocatable addresses in memory with the actual addresses
 * of the symbols they refer to. If a symbol is not found, it marks the word as external.
 */
static void update_symbol_addresses(void) {
    int i, j;
    Symbol* symbol;
    MachineWord word;
    int string_index;
    int found_extern;

    printf("DEBUG: Updating symbol addresses\n");
    printf("DEBUG: String table contents:\n");
    for (i = 0; i < string_count; i++) {
        printf("DEBUG: Index %d: '%s'\n", i, string_table[i]);
    }

    for (i = 0; i < IC + DC - INITIAL_MEMORY_ADDRESS; i++) {
        word = memory[i];

        if ((word & 3) == ARE_RELOCATABLE) {
            string_index = word >> 2;
            printf("DEBUG: Processing word at address %d, string index: %d\n", i + INITIAL_MEMORY_ADDRESS, string_index);
            if (string_index < string_count) {
                printf("DEBUG: Looking up symbol: '%s'\n", string_table[string_index]);
                symbol = get_symbol_by_name(string_table[string_index]);
                if (symbol != NULL) {
                    memory[i] = ((symbol->address & 0xFFF) << 3) | ARE_RELOCATABLE;
                    printf("DEBUG: Symbol found in symbol table, address: %d\n", symbol->address);
                } else {
                    /* Check if it's an external symbol */
                    found_extern = 0;
                    for (j = 0; j < extern_count; j++) {
                        printf("DEBUG: Comparing '%s' with extern '%s'\n", string_table[string_index], externs[j].name);
                        if (strcmp(string_table[string_index], externs[j].name) == 0) {
                            found_extern = 1;
                            memory[i] = ARE_EXTERNAL;
                            add_extern_use(externs[j].name, i + INITIAL_MEMORY_ADDRESS);
                            printf("DEBUG: External symbol found and added to extern uses\n");
                            break;
                        }
                    }
                    if (!found_extern) {
                        add_error(ERROR_UNDEFINED_LABEL, current_filename, -1,
                                  "Symbol %s not found in symbol table or externals",
                                  string_table[string_index]);
                        printf("DEBUG: Symbol not found in symbol table or externals\n");
                    }
                }
            } else {
                add_error(ERROR_INVALID_OPERAND, current_filename, -1,
                          "Invalid string table index");
                printf("DEBUG: Invalid string table index\n");

            }
        }
    }

    /* Handle entry symbols */
    for (i = 0; i < entry_count; i++) {
        symbol = get_symbol_by_name(entries[i].name);
        if (symbol != NULL) {
            set_entry_address(entries[i].name, symbol->address);
        } else {
            add_error(ERROR_UNDEFINED_LABEL, current_filename, -1,
                      "Entry symbol '%s' not found in symbol table", entries[i].name);
        }
    }
}

/*
 * Creates the output files for object, externals, and entries.
 * This function generates the filenames based on the base name of the input file
 * and writes the corresponding content to each file.
 */
static void create_output_files(const char *filename) {
    char *base_filename = get_filename_without_extension(filename);
    write_object_file(base_filename);
    write_externals_file(base_filename);
    write_entries_file(base_filename);
    free(base_filename);
}

/*
 * Writes the object file with the current machine memory state.
 * This file includes addresses and octal representations of machine words.
 */
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

/*
 * Writes the externals file listing external symbols and their locations.
 * The file contains the name of each external symbol and its address in memory.
 */
static void write_externals_file(const char *filename) {
    char ext_filename[MAX_FILENAME_LENGTH];
    FILE *ext_file;
    int i;

    if (extern_use_count > 0) {
        strcpy(ext_filename, filename);
        strcat(ext_filename, EXTERNALS_FILE_EXT);
        ext_file = safe_fopen(ext_filename, "w");

        for (i = 0; i < extern_use_count; i++) {
            fprintf(ext_file, "%s %04d\n", extern_uses[i].name, extern_uses[i].address);
        }

        fclose(ext_file);
    }
}

/*
 * Writes the entries file with entry symbols and their addresses.
 * The file contains the name and address of each entry symbol.
 */
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

/*
 * Converts a number to its octal representation.
 * This function takes a 16-bit number and returns its octal string representation.
 */
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

/*
 * Gets the filename without its extension.
 * This function removes the file extension from the given filename and returns the base name.
 */
static char *get_filename_without_extension(const char *filename) {
    BaseFilename base_filename = get_base_filename(filename);
    char *result = (char *) safe_malloc(base_filename.length + 1);

    strncpy(result, base_filename.name, base_filename.length);
    result[base_filename.length] = '\0';

    free(base_filename.name);
    return result;
}
