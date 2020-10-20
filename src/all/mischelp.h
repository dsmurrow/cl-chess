#ifndef MISCHELP_H_INCLUDED
#define MISCHELP_H_INCLUDED

#include "chess.h"
#include "logichelp.h"

typedef struct
{
    Piece *p;
    Location loc;
} Move;

uintmax_t square(intmax_t);

void makeColor(int_fast8_t, int_fast8_t);
uint_fast8_t u_8(uint_fast8_t, uint_fast8_t);

void ClearScreen();

void to_PGN(char*, Game*, Piece*, Location, int_fast8_t);
void post_PGN(char*, Game*, char);

Move decipher_move(Game*, int_fast8_t, char*, int_fast8_t);


#endif /* MISCHELP_H_INCLUDED */
