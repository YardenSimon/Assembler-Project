#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* name;
    size_t length;
} BaseFilename;


/* Skips whitespace characters in the given string.
 * Moves the string pointer forward past any whitespace characters. */
void skip_whitespace(char** str);

/* Extracts the base filename (excluding path and extension) from a full filename.
 * Allocates memory for the base name and returns it along with its length. */
BaseFilename get_base_filename(const char* filename);

/*  Safely allocates memory of the specified size.
 * Exits the program if memory allocation fails. */
void* safe_malloc(size_t size);

/* Safely opens a file with the given filename and mode.
 * Exits the program if the file cannot be opened.  */
FILE* safe_fopen(const char* filename, const char* mode);

/* Safely reads a line from a file into the provided buffer.
 * Exits the program if reading from the file fails.*/
char* safe_fgets(char* str, int n, FILE* stream);

/* Converts a string to an integer.
 * Checks for errors in conversion and range, and exits the program if errors are found. */
int safe_atoi(const char* str);

/* Checks if an address is in the data section.
 * It looks through all data sections to see if the address fits in any of them.
 * Returns 1 if the address is in a data section, 0 if not.
 */

int is_data_address(int address);

#endif