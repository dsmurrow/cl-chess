#include "logichelp.h"

/**
 * Captures piece i.e. frees the location of the piece and
 * sets it to NULL.
 *
 * @param board The game currently being played
 * @param p The piece being captured
 * @param index The index of the piece in the board's pieceLocations array
 */
void capture(Game *board, Piece *p)
{
	int_fast8_t i = (p->color - 1) * 16;
	while(i < 2 * PIECES_PER_SIDE && 
		  (board->pieceLocations[i] == 0 || ( board->pieceLocations[i] != 0 && *(board->pieceLocations[i]) != p->currentLocation ) ))
		i++;


	p->currentLocation = 0;
}

/**
 * Reverses a capture if the move was invalid.
 *
 * @param board      The Game instance being played
 * @param p          The piece being uncaptured
 * @param index      The index of p in its color array
 * @param coords     The coordinates that p is going to get
 */
void uncapture(Game *board, Piece *p, uint_fast8_t index, Location coords)
{
	uint_fast8_t realindex = (p->color - 1) * 16 + index;

	assert(p->currentLocation == 0);
	assert(*(board->pieceLocations[realindex]) == 0);

	p->currentLocation = coords;
}

/**
 * Deduces if there is a piece residing on the given location.
 *
 * @param board The game instance being played
 * @param loc The location being tested
 *
 * @return 0 if there is no piece on the board, TEAM_WHITE if
 *           there is a white piece on the board, or TEAM_BLACK
 *           if there is a black piece on the board
 */
int_fast8_t piece_is_on(Game *board, const Location loc)
{
	int_fast8_t ret = 0;

	Location **locs = board->pieceLocations;

	uint_fast8_t h = location_getfile(loc);
	uint_fast8_t v = location_getrank(loc);

	uint_fast8_t i;
	for(i = 0; IS_ON_BOARD(h, v) && i < 2 * PIECES_PER_SIDE; i++)
	{
		/* if the piece is on the board and its coordinates match loc's */
		if(locs[i] != 0 && location_equals_coords(*(locs[i]), h, v))
		{
			ret = i < PIECES_PER_SIDE ? TEAM_WHITE : TEAM_BLACK;
			break;
		}
	}

	return ret;
}

/**
 * With the given information, determines if there are any chess pieces between two locations.
 *
 * @param board    The current game instance being played
 * @param oldLoc   The starting point for the function's analysis
 * @param newLoc   The ending point of the analysis
 * @param xInc     The value the horizontal axis is being incremented by each iteration
 * @param yInc     The value the vertical axis is being incremented by each iteration
 *
 * @return         True if a piece was anywhere between oldLoc and newLoc. False otherwise
 */
bool path_is_blocked(Game *board, const Location oldLoc, const Location newLoc, const int_fast8_t xInc, const int_fast8_t yInc) /* Mainly used for bishops, rooks, queens */
{
	bool blocked = false;

	int_fast8_t x;
	int_fast8_t y;

	for(x=location_getfile(oldLoc) + xInc, y=location_getrank(oldLoc) + yInc; x!=location_getfile(newLoc) || y!=location_getrank(newLoc); x+=xInc, y+=yInc)
	{
		Location temp;
		location_assign(&temp, x, y);

		if(piece_is_on(board, temp))
		{
			blocked = true;
			break;
		}
	}

	return blocked;
}

static bool check_if_mate(Game *board, Piece **team)
{
	Piece *CheckedKing = team[I_KING];

	bool ret = false;
	if(!is_valid_move(board, CheckedKing, CheckedKing->currentLocation, VALID_SELFCHECK))
	{
		Piece **enemies = CheckedKing->color == TEAM_WHITE ? board->Black : board->White;
		Piece *current;
		uint_fast8_t i, x, y;
		ret = true;

		for(i = I_PAWN1; ret && i <= I_ROOK2; i++)
		{
			current = team[i];

			for(x = 1; ret && current->currentLocation != 0 && x < 9; x++)
			{
				for(y = 1; ret && y < 9; y++)
				{
					Location check;
					location_assign(&check, x, y);

					if(is_valid_move(board, current, check, 0))
					{
						bool enemyOn;
						int_fast8_t enemyIndex;
						Piece *enemy;
						Location old;

						enemyOn = piece_is_on(board, check) == enemies[0]->color;

						if(enemyOn)
						{
							int_fast8_t j;
							bool match = false;
							for(j = I_PAWN1; !match && j <= I_ROOK2; j++)
								if(enemies[j]->currentLocation != 0 && enemies[j]->currentLocation == check)
									match = true;
							assert(match);

							enemyIndex = --j;
							enemy = enemies[j];

							capture(board, enemy);
						}

						old = current->currentLocation;

						current->currentLocation = check;

						if(is_valid_move(board, CheckedKing, CheckedKing->currentLocation, VALID_SELFCHECK))
							ret = false;

						current->currentLocation = old;

						if(enemyOn) uncapture(board, enemy, enemyIndex, check);
					}
				}
			}
		}
	}

	return ret;
}

int_fast8_t check_if_check(Game *board)
{
	uint_fast8_t ret;    

	const Piece *WhiteKing = board->White[I_KING];
	const Piece *BlackKing = board->Black[I_KING];

	const bool WKingValid = is_valid_move(board, WhiteKing, WhiteKing->currentLocation, VALID_SELFCHECK);
	const bool BKingValid = is_valid_move(board, BlackKing, BlackKing->currentLocation, VALID_SELFCHECK);

	const bool BOTH_KINGS_ARE_IN_CHECK = WKingValid || BKingValid;
	assert(BOTH_KINGS_ARE_IN_CHECK);


	if(WKingValid ^ BKingValid)
	{
		bool mate;
		ret = CHECK_YES;

		mate = check_if_mate(board, !WKingValid ? board->White : board->Black);

		ret = mate ? ret | CHECK_MATE : ret;
	}
	else
		ret = CHECK_NO;


	return ret;
}
