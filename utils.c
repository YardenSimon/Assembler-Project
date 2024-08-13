/* utils.c */

#include "utils.h"
#include "globals.h"
#include <ctype.h>
#include <string.h>

void skip_whitespace(char** str) {
    while (**str && isspace((unsigned char)**str)) {
        (*str)++;
    }
}

BaseFilename get_base_filename(const char* filename) {
    BaseFilename result;
    const char* base_name;
    const char* dot_position;

    base_name = strrchr(filename, '/');
    if (base_name == NULL) {
        base_name = filename;
    } else {
        base_name++;  /* Skip the '/' */
    }

    dot_position = strrchr(base_name, '.');
    if (dot_position != NULL) {
        result.length = dot_position - base_name;
    } else {
        result.length = strlen(base_name);
    }

    result.name = (char*)safe_malloc(result.length + 1);
    strncpy(result.name, base_name, result.length);
    result.name[result.length] = '\0';

    return result;
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

FILE* safe_fopen(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    return file;
}

char* safe_fgets(char* str, int n, FILE* stream) {
    if (fgets(str, n, stream) == NULL) {
        if (ferror(stream)) {
            fprintf(stderr, "Error: Failed to read from file\n");
            exit(EXIT_FAILURE);
        }
        return NULL;  /* EOF reached */
    }
    return str;
}

int safe_atoi(const char* str) {
    char* endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str) {
        fprintf(stderr, "Error: No digits were found in '%s'\n", str);
        exit(EXIT_FAILURE);
    }

    if (*endptr != '\0') {
        fprintf(stderr, "Error: Invalid character in '%s' after the number\n", str);
        exit(EXIT_FAILURE);
    }

    if (val > INT_MAX || val < INT_MIN) {
        fprintf(stderr, "Error: Number '%s' is out of range for an integer\n", str);
        exit(EXIT_FAILURE);
    }

    return (int)val;
}