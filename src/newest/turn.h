#ifndef TURN_H_INCLUDED
#define TURN_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "char.h"

typedef struct turn
{
    uintmax_t number;
    char *White;
    char *Black;
    struct turn *next;
} Turn;

typedef struct move_list
{
    uintmax_t num;
    Turn *firstMove;
    Turn *LatestMove;
} MoveList;

Turn *get_move_number(MoveList, uintmax_t);

void add_move(MoveList*, char*);
void update_latest_move(MoveList*, char*);
void remove_latest_move(MoveList*);

void print_moves(MoveList);

#endif /* TURN_H_INCLUDED */
