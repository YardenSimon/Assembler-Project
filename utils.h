/* utils.h */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* name;
    size_t length;
} BaseFilename;


/* Function to skip whitespace in a string */
void skip_whitespace(char** str);

/* Function to get the base filename without extension */
BaseFilename get_base_filename(const char* filename);

/* Function for safe memory allocation */
void* safe_malloc(size_t size);

void* safe_realloc(void* ptr, size_t size);

/* Function for safe file opening */
FILE* safe_fopen(const char* filename, const char* mode);

/* Function to safely read a line from a file */
char* safe_fgets(char* str, int n, FILE* stream);

/* Function to safely convert a string to an integer */
int safe_atoi(const char* str);

#endif /* UTILS_H */