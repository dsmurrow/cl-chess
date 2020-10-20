#ifndef FILEREADING_H_INCLUDED
#define FILEREADING_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "char.h"

uintmax_t count_occurences_of_char_in_file(char, const char*);

char **get_moves_from_file(const char*, uintmax_t*);

void readfile(const char*, char*);


#endif /* FILEREADING_H_INCLUDED */
