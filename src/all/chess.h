#ifndef CHESS_H_INCLUDED
#define CHESS_H_INCLUDED

#include <time.h>

#include "piece.h"

#ifdef __WIN32
	#include <windows.h>
	#define RESETCOLOR makeColor(WHITE, BLACK)
#else
	#define RESETCOLOR printf("\033[0m")
#endif

typedef struct player Player;
typedef struct game Game;



struct game
{
	MoveList Moves;
	Piece *White[PIECES_PER_SIDE];
	Piece *Black[PIECES_PER_SIDE];
	Location *pieceLocations[2 * PIECES_PER_SIDE]; 	/* when a piece is captured set it's location to NULL, and when */
};													/* checking element in this, first check if the element is NULL */


Game *init_game();
void free_game(Game*);
void print_board(Game*, int_fast8_t);
void print_pieces(Game*, int_fast8_t);
bool is_valid_move(Game*, const Piece*, const Location, int_fast8_t);

int_fast16_t process_move(Game*, const char*, int_fast8_t);

#endif /* CHESS_H_INCLUDED */
