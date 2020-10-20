#ifndef LOGICHELP_H_INCLUDED
#define LOGICHELP_H_INCLUDED

#include "mischelp.h"

void capture(Game*, Piece*);
void uncapture(Game*, Piece*, uint_fast8_t, Location);
int_fast8_t piece_is_on(Game*, const Location);
int_fast8_t check_if_check(Game*);
bool path_is_blocked(Game*, const Location, const Location, const int_fast8_t, const int_fast8_t);

#endif /* LOGICHELP_H_INCLUDED */
