#include "piece.h"

/**
 * Returns the name of the type of piece given in the input.
 *
 * @param p   The piece being used to determine the string
 *
 * @return    The name of the type of piece p is
 */
char *get_piece_name(Piece *p)
{
    char *pieceName;

    switch(p->type)
    {
        case PIECE_PAWN:
            pieceName = "Pawn";
            break;
        case PIECE_BISHOP:
            pieceName = "Bishop";
            break;
        case PIECE_KNIGHT:
            pieceName = "Knight";
            break;
        case PIECE_ROOK:
            pieceName = "Rook";
            break;
        case PIECE_KING:
            pieceName = "King";
            break;
        case PIECE_QUEEN:
            pieceName = "Queen";
            break;
        default:
            pieceName = "Unknown";
    }
    return pieceName;
}

char get_piece_symbol(Piece *p)
{
    char ret;

    switch(p->type)
    {
        case PIECE_PAWN:
            ret = location_getfile(p->currentLocation) + 96;
            break;
        case PIECE_BISHOP:
            ret = 'B';
            break;
        case PIECE_ROOK:
            ret = 'R';
            break;
        case PIECE_KNIGHT:
            ret = 'N';
            break;
        case PIECE_KING:
            ret = 'K';
            break;
        case PIECE_QUEEN:
            ret = 'Q';
            break;
    }

    return ret;
}

char get_piece_icon(const Piece p)
{
	char ret = '?';

	switch(p.type)
	{
		case PIECE_PAWN:
			ret = 'P';
			break;
		case PIECE_BISHOP:
			ret = 'B';
			break;
		case PIECE_KNIGHT:
			ret = 'N';
			break;
		case PIECE_ROOK:
			ret = 'R';
			break;
		case PIECE_KING:
			ret = 'K';
			break;
		case PIECE_QUEEN:
			ret = 'Q';
			break;
	}

	return ret;
}

/**
 * Puts all of the piece in black and white into a single array.
 *
 * @param board   The game instance being played
 *
 * @return        An array containing every piece belonging to both sides
 */
void get_all_pieces(Piece *All[], Piece *White[], Piece *Black[])
{
    uint_fast8_t i, j;

    for(i = 0, j = 0; i < PIECES_PER_SIDE; i++, j++)
    {
        All[i] = White[j];
        All[i + PIECES_PER_SIDE] = Black[j];
    }
}

/**
 * Returns a pointer to the piece that resides at the given location.
 *
 * @param all    An array containing all of the pieces
 * @param loc    The location where a piece should be
 *
 * @returns the piece at loc
 */
Piece *piece_at(Piece **all, Location loc)
{
    uint_fast8_t i;
    Piece *ret = NULL;

    for(i = 0; ret == NULL && i < 2 * PIECES_PER_SIDE; i++)
        if(all[i]->currentLocation != 0 && all[i]->currentLocation == loc)
            ret = all[i];

    return ret;
}


char piece_promoted(Piece *p, const char *analString)
{
    char ret = 0;

    uint_fast8_t endVert = p->color == TEAM_WHITE ? 8 : 1;
    if(p->type == PIECE_PAWN && p->currentLocation != 0 && location_getrank(p->currentLocation) == endVert)
    {
        uint_fast8_t i;
        char pieces[4] = {'N', 'R', 'B', 'Q'};

        ret = 'Q';
        for(i = 0; ret == 'Q' && i < 4; i++)
            if(string_contains(analString, pieces[i]))
                ret = pieces[i];

        switch(ret)
        {
            case 'Q':
                p->type = PIECE_QUEEN;
                break;
            case 'R':
                p->type = PIECE_ROOK;
                break;
            case 'N':
                p->type = PIECE_KNIGHT;
                break;
            case 'B':
                p->type = PIECE_BISHOP;
                break;
        }
    }

    return ret;
}
