#include "utils.h"
#include "errors.h"
#include "globals.h"
#include <ctype.h>
#include <string.h>
#include <limits.h>


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
        base_name++;
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
        add_error(ERROR_MEMORY_ALLOCATION, current_filename, -1, "Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    return ptr;
}


FILE* safe_fopen(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        add_error(ERROR_FILE_NOT_FOUND, filename, -1, "Unable to open file '%s'", filename);
        exit(EXIT_FAILURE);
   }
    return file;
}


char* safe_fgets(char* str, int n, FILE* stream) {
    if (fgets(str, n, stream) == NULL) {
        if (ferror(stream)) {
            add_error(ERROR_FILE_NOT_FOUND, current_filename, -1, "Failed to read from file");
            exit(EXIT_FAILURE);
        }
        return NULL;
    }
    return str;
}


int safe_atoi(const char* str) {
    char* endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str) {
        add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "No digits were found in '%s'", str);
        return 0;
    }

    if (*endptr != '\0') {
        add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Invalid character in '%s' after the number", str);
        return 0;
    }

    if (val > INT_MAX || val < INT_MIN) {
        add_error(ERROR_INVALID_OPERAND, current_filename, current_line_number, "Number '%s' is out of range for an integer", str);
        return 0;
    }

    return (int)val;
}


int is_data_address(int address) {
    int i;

    for (i = 0; i < data_section_count; i++) {
        if (address >= data_sections[i].start_address && address <= data_sections[i].end_address) {
            return 1;
        }
    }
    return 0;
}