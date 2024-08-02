/* macros.h */

#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>

#define MAX_MACRO_NAME 31
#define MAX_MACRO_CONTENT 1000

/* Structure to store macro information */
typedef struct {
    char name[MAX_MACRO_NAME];
    char* content;
} Macro;

/* Function declarations */
void init_macros();
int word_in_list(const char *word, const char *list[], int list_count);
int can_be_macro_name(const char *word);
void handle_macro_inside(const char *macro_name, FILE *file);
void replace_macros(const char *input_name, const char *output_name);
void free_macros();

/* External declarations for word groups */
extern const char *group1[];
extern const int group1_count;
extern const char *group2[];
extern const int group2_count;
extern const char *group3[];
extern const int group3_count;
extern const char *instruction_words[];
extern const int instruction_words_count;

#endif /* MACROS_H */