#ifndef PIECE_H_INCLUDED
#define PIECE_H_INCLUDED

#include "location.h"
#include "macros.h"

typedef struct piece
{
    uint_fast8_t color;
    uint_fast8_t type;
    bool hasMoved;
    Location currentLocation;
} Piece;


char piece_promoted(Piece*, const char*);
char *get_piece_name(Piece*);
char get_piece_symbol(Piece*);
char get_piece_icon(const Piece);
void get_all_pieces(Piece*[], Piece*[], Piece*[]);

Piece *piece_at(Piece**, Location);

#endif /* PIECE_H_INCLUDED */
