#ifndef CHAR_H_INCLUDED
#define CHAR_H_INCLUDED

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

bool char_is_coord(char);
bool char_is_digit(char);
bool char_is_piece(char);
bool char_array_containts(const char*, uintmax_t, char);

uintmax_t string_getlen(const char*);
uintmax_t string_add_char(char*, char);
uintmax_t string_concatenate(char*, const char*);
void string_remove(char*, const uintmax_t);
void string_copy(char*, const char*);

void tokenize_move(char*);

void string_tolower(char*);
bool string_matches(const char*, const char*);
bool string_matches_end(const char*);
bool string_contains(const char*, char);
uintmax_t string_count_occurences_of_char(const char*, char);
uintmax_t string_split(char*, uintmax_t, const char*, char);
uintmax_t string_split_malloc(char**, const char*, char);

#endif /* CHAR_H_INCLUDED */
