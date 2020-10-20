#include "chess.h"
#include "mischelp.h"
#include "logichelp.h"

double functime = 0;

/**
 * Initializes a new game object.
 *
 * @return New game board with pieces in starting positions
 */
Game *init_game()
{
    uint_fast8_t h, v, pieceType, i, j;
    bool isWhite;
    Game *game;
    Piece *current;
    Location loc;

    /* Initialize Pieces */
    game = malloc(sizeof(Game));

    for(i = 0; i < 2 * PIECES_PER_SIDE; i++)
    {
        j = i % 16; /* j is the true index in each color's respective array */
        isWhite = i < 16 ? true : false;

        if(j == I_ROOK1 || j == I_ROOK2)
            pieceType = PIECE_ROOK;
        else if(j == I_KNIGHT1 || j == I_KNIGHT2)
            pieceType = PIECE_KNIGHT;
        else if(j == I_BISHOP1 || j == I_BISHOP2)
            pieceType = PIECE_BISHOP;
        else if(j == I_QUEEN)
            pieceType = PIECE_QUEEN;
        else if(j == I_KING)
            pieceType = PIECE_KING;
        else
            pieceType = PIECE_PAWN;


        v = 2 - (j / 8); /* vertical position relative to the bottom of the board */
        h = (j % 8) + 1;

        current = isWhite ? (game->White[j] = malloc(sizeof(Piece))) : (game->Black[j] = malloc(sizeof(Piece)));

        current->color = isWhite ? TEAM_WHITE : TEAM_BLACK;
        current->type = pieceType;
        current->hasMoved = false;

		location_assign(&(current->currentLocation), h, isWhite ? v : 9 - v);
  
        assert(location_getrank(current->currentLocation) <= 2 || location_getrank(current->currentLocation) >= 7);
        assert(location_getfile(current->currentLocation) > 0 && location_getfile(current->currentLocation) < 9);


        game->pieceLocations[i] = &(current->currentLocation);
    }

    game->Moves.num = 0;
    game->Moves.firstMove = NULL;
    game->Moves.LatestMove = NULL;

    return game;
}

/**
 * Frees all the memory that was dynamically allocated over the course of the program.
 *
 * @param board   The Game instance being freed
 */
void free_game(Game *board)
{
    uint_fast8_t i, j;
    Turn *current, *previous;

    for(i = 0; i < 2 * PIECES_PER_SIDE; i++)
    {
		Piece *current;

        j = i % PIECES_PER_SIDE;
        current = i < PIECES_PER_SIDE ? board->White[j] : board->Black[j];

        free(current);
    }

    /* Free the moves list */
    current = board->Moves.firstMove;
    previous = NULL;
    while(current != NULL)
    {
        free(current->White);
        if(current->Black != NULL) free(current->Black);

        previous = current;

        current = previous->next;

        if(previous != NULL)  free(previous);
    }

    free(board);
}

/**
 * Determines if a piece going to a specified location is legal under chess rules.
 *
 * @param board     The current game instance being played
 * @param piece     The chess piece that is moving
 * @param newPlace  The location that piece is moving to
 * @param flags     Flags for the function
 *
 * @return          True if the move is valid, false if move is invalid
 */
bool is_valid_move(Game *board, const Piece *piece, const Location newPlace, int_fast8_t flags)
{
    bool ret;
    int_fast8_t isOn;
    uint_fast8_t newH, newV;

    if(flags & VALID_BROADCASTCALL) printf("is_valid_move() was called (color = %" PRIuFAST8 "). newPlace = %c%" PRIuFAST8 "\n", piece->color, (char)(location_getfile(newPlace) + 96), location_getrank(newPlace));

    ret = false;

    isOn = (flags & VALID_IGNORECOLOR) || (flags & VALID_SELFCHECK) ? 0 : piece_is_on(board, newPlace);
    if(flags & VALID_BROADCASTCALL) printf("is_valid_move(): isOn = %" PRIuFAST8 "\n", isOn);

    newH = location_getfile(newPlace);
    newV = location_getrank(newPlace);



    /*
     *  Checks if 1) The new location is on the board at all,
     *  2) The new location isn't the same as the piece's current location, and
     *  3) If there is a piece in the new spot, it's not the same color as the piece that's moving
     */
    if(piece->currentLocation != 0 && IS_ON_BOARD(newH, newV) && isOn != piece->color)
    {
        int_fast8_t deltX, deltY;

        if(flags & VALID_BROADCASTCALL) printf("Location is on board\n");

        deltX = flags & VALID_KINGNOLIMIT ? 1 : location_getfile(newPlace) - location_getfile(piece->currentLocation);
        deltY = flags & VALID_KINGNOLIMIT ? 1 : location_getrank(newPlace) - location_getrank(piece->currentLocation);

        if(flags & VALID_BROADCASTCALL) printf("created variables in is_valid_move()\n");

        if(piece->type == PIECE_PAWN)
        {
            const Location here = piece->currentLocation;
            const Location nl = newPlace;

            const int_fast8_t isWhite = -((piece->color * 2) - 3);

            const int_fast8_t FORWARD = location_getrank(here) + isWhite;
            const int_fast8_t FORWARD2 = FORWARD + isWhite;
            const int_fast8_t LEFT = location_getfile(here) + -isWhite;
            const int_fast8_t RIGHT = location_getfile(here) + isWhite;

            assert(isWhite == (piece->color == TEAM_WHITE ? 1 : -1));
            if(flags & VALID_BROADCASTCALL) printf("reached pawns. here = %X, nl = %X\n", here, nl);


            if(location_getrank(nl) == FORWARD)
            {
                if(flags & VALID_BROADCASTCALL) printf("nl->vertical == FORWARD\n");
                if(!(flags & VALID_PAWNATKONLY) && location_getfile(nl) == location_getfile(here) && !piece_is_on(board, nl))
                    ret = true;

                else if(location_getfile(nl) == LEFT || location_getfile(nl) == RIGHT)
                {
                    if(flags & VALID_BROADCASTCALL) printf("diagonal\n");

                    if((flags & VALID_IGNORECOLOR) || piece_is_on(board, nl) || (flags & VALID_PAWNATKONLY))
                        ret = true;


                    /* en passant */
                    else if(board->Moves.firstMove != NULL)
                    {
						char *last;
						bool is_a_two_mover;
		 				Turn *current;

                        if(flags & VALID_BROADCASTCALL) printf("en passant, LatestMove = (%" PRIuMAX "\t%s\t%s)\n", board->Moves.LatestMove->number, board->Moves.LatestMove->White, board->Moves.LatestMove->Black);
                        last = piece->color == TEAM_BLACK ? board->Moves.LatestMove->White : board->Moves.LatestMove->Black;
                        if(flags & VALID_BROADCASTCALL) printf("got last (%s)\n", last);

                        is_a_two_mover = string_getlen(last) == 2 && last[0] - 96 == location_getfile(nl) && last[1] - '0' == location_getrank(here);

                        current = board->Moves.firstMove;
                        if(flags & VALID_BROADCASTCALL) printf("is_a_two_mover = %i, trn will have to look like %c%c, last = %s\n", is_a_two_mover, (char)(location_getfile(nl) + 96), FORWARD + '0', last);
                        while(is_a_two_mover && current != NULL)
                        {
                            char *trn = piece->color == TEAM_BLACK ? current->White : current->Black;

                            if(string_getlen(trn) == 2 && trn[0] - 96 == location_getfile(nl) && trn[1] - '0' == FORWARD)
                                is_a_two_mover = false;


                            current = current->next;
                        }
                        if(flags & VALID_BROADCASTCALL) printf("out of two_mover loop");
                        if(is_a_two_mover)
                            ret = true;
                        if(flags & VALID_BROADCASTCALL) printf("end of en passant\n");
                    }

                }

            }
            else if(!(flags & VALID_PAWNATKONLY) && !(piece->hasMoved) &&           /* e5 */
                    location_getrank(nl) == FORWARD2 && location_getfile(nl) == location_getfile(here) &&
                    !path_is_blocked(board, here, nl, 0, isWhite))
            {
                if(flags & VALID_BROADCASTCALL) printf("FORWARD2 block reached\n");
                ret = true;
            }
        }

        /* BISHOPS, ROOKS, KNIGHTS, & QUEENS */
        else if(piece->type & PIECE_BISHOP || piece->type & PIECE_ROOK || piece->type == PIECE_KNIGHT)
        {
	    	int_fast8_t xInc, yInc, intslope;


            if(flags & VALID_BROADCASTCALL) printf("reached bish/queen/rook/knight part\n");
            xInc = deltX >= -1 && deltX <= 1 ? deltX : (deltX > 1 ? 1 : -1);
            yInc = deltY >= -1 && deltY <= 1 ? deltY : (deltY > 1 ? 1 : -1);



            if(deltX == 0 || deltY == 0)
                intslope = 0;
            else if(square(deltX) == square(deltY))
                intslope = 1;
            else if(((square(deltX) == 1) ^ (bool)(square(deltY) == 1)) && ((square(deltX) == 4) ^ (bool)(square(deltY) == 4)))
                intslope = 2;

            if(flags & VALID_BROADCASTCALL) printf("Slope calculated\n");

            if( ((intslope == 1 && piece->type & PIECE_BISHOP) || (intslope == 0 && piece->type & PIECE_ROOK)) &&
                !path_is_blocked(board, piece->currentLocation, newPlace, xInc, yInc) )
                ret = true;
            else if(square(deltX) <= 4 && square(deltY) <= 4 && piece->type == PIECE_KNIGHT && intslope == 2)
                ret = true;
        }


        /* KING */
        else if(piece->type == PIECE_KING && square(deltX) <= 1 && square(deltY) <= 1)
        {
            if(flags & VALID_BROADCASTCALL) printf("Got to king\n");

            ret = true;
            if(!(flags & VALID_IGNORECOLOR))
            {
                Piece **EnemyPieces = piece->color == TEAM_WHITE ? board->Black : board->White;
                uint_fast8_t i;
                for(i = 0; ret && i < PIECES_PER_SIDE; i++)
                {
                    /* Check if any enemy pieces can attack the new location. Then checks if that piece is on the new location. */
                    if(is_valid_move(board, EnemyPieces[i], newPlace, VALID_IGNORECOLOR | VALID_PAWNATKONLY) &&
                       EnemyPieces[i]->currentLocation != newPlace)
                    {
                        if(flags & VALID_BROADCASTCALL) printf("not valid king move: i = %" PRIuFAST8 "\n", i);
                        ret = false;
                    }
                }
            }
        }
        /* CASTLING */
        else if(piece->type == PIECE_KING && !(piece->hasMoved) &&
                (newH == 3 || newH == 7) && deltY == 0)
        {
	    bool kingCanGo;
	    int_fast8_t inc;
	    uint_fast8_t i, rookIndex;
	    Piece *rook;


            if(flags & VALID_BROADCASTCALL) printf("got to castle\n");
            rookIndex = newH == 3 ? I_ROOK1 : I_ROOK2;
            rook = piece->color == TEAM_WHITE ? board->White[rookIndex] : board->Black[rookIndex];

            if(flags & VALID_BROADCASTCALL) printf("castling rook initialized in is_valid_move()\n");

            kingCanGo = true;
            inc = rookIndex == I_ROOK1 ? -1 : 1;
            for(i = 5; kingCanGo && i != newH; i += inc)
            {
		Location temp;

                if(flags & VALID_BROADCASTCALL) printf("castle loops\n");

		location_assign(&temp, i, newV);


                assert(IS_ON_BOARD(i, newV));
                if(!is_valid_move(board, piece, temp, VALID_KINGNOLIMIT | VALID_SELFCHECK | (flags & VALID_BROADCASTCALL)))
                {
                    kingCanGo = false;
                }
            }
            if(flags & VALID_BROADCASTCALL) printf("Completed castle loops: kingCanGo = %i\n", kingCanGo);

            if(kingCanGo && !(rook->hasMoved) && !path_is_blocked(board, rook->currentLocation, newPlace, -inc, 0))
                ret = true;
        }

        else if(flags & VALID_BROADCASTCALL) printf("Piece not caught\n");
    }
    else if(flags & VALID_BROADCASTCALL) printf("Piece is NOT on the board\n");

    if(flags & VALID_BROADCASTCALL) printf("Move is %s valid\n", ret ? "" : "NOT");

    return ret;
}

/**
 * Given a string representing the user's desired move, this function will either do nothing
 * if the move is invalid, or it will perform the requested move if it is legal.
 *
 * @param board   The Game instance being played
 * @param inStr   The string the user gave to express their desired move
 * @param flags   Flags for the function
 *
 * @returns true if the move was valid and a piece was moved, false if the move was invalid and nothing moved
 */
int_fast16_t process_move(Game *board, const char *inStr, int_fast8_t flags)
{
    uintmax_t start;
    int_fast16_t moved;
    int_fast8_t whosTurnIsIt;
    char moveStr[9];
    Move deciphered;


    if(flags & MOVE_RUNTIME) start = clock();

    moved = 0;
    whosTurnIsIt = board->Moves.LatestMove == NULL || board->Moves.LatestMove->Black != NULL ? TEAM_WHITE : TEAM_BLACK;

    if(flags & MOVE_BROADCAST) printf("moved = false and whosTurnIsIt = %" PRIdFAST8 "\n", whosTurnIsIt);

    string_copy(moveStr, inStr);

    if(flags & MOVE_BROADCAST) printf("Move about to be deciphered\n");
    
    deciphered = decipher_move(board, whosTurnIsIt, moveStr, flags & DECIPHER_BROADCAST);
   
    if(flags & MOVE_BROADCAST) printf("Move deciphered\n");


    if(deciphered.p != NULL && is_valid_move(board, deciphered.p, deciphered.loc, flags & VALID_BROADCASTCALL))
    {
	    bool isCastle;
	    char PGNMule[10];
	    uint_fast8_t at_index;
	    Location old, destination, isOnLoc;
	    Piece *at, *p, *rook, *pKing, *All[2 * PIECES_PER_SIDE], **team;


        if(flags & MOVE_BROADCAST) printf("deciphered.p != NULL and move is valid\n");

        to_PGN(PGNMule, board, deciphered.p, deciphered.loc, flags & PGN_BROADCAST);

        get_all_pieces(All, board->White, board->Black);

        if(flags & MOVE_BROADCAST) printf("all pieces gotten\n");

	    location_assign(&isOnLoc, PGNMule[2] - 96, location_getrank(deciphered.p->currentLocation));

    	at = NULL;
    	if(string_contains(PGNMule, 'x') && piece_is_on(board, deciphered.loc))
    	{
        	at = piece_at(All, deciphered.loc);
        	assert(at != NULL);

        	for(at_index = 0; All[at_index] != at; at_index++);
        	at_index %= 16;

        	capture(board, at);
    	}
    	else if(string_contains(PGNMule, 'x') && !piece_is_on(board, deciphered.loc) && deciphered.p->type == PIECE_PAWN
    			&& piece_is_on(board, isOnLoc))
    	{
	    	Location check;
        	if(flags & MOVE_BROADCAST) printf("en passant part of process move reached\n");

	    	location_assign(&check, PGNMule[2] - 96, location_getrank(deciphered.p->currentLocation));

        	at = piece_at(All, check);
        	assert(at != NULL);
        	assert(at->type == PIECE_PAWN);

        	if(flags & MOVE_BROADCAST) printf("at gotten\n");

        	for(at_index = 0; All[at_index] != at; at_index++);

        	at_index %= 16;

        	if(flags & MOVE_BROADCAST) printf("at_index = %" PRIdFAST8 "\n", at_index);

        	capture(board, at);
    	}

    	if(flags & MOVE_BROADCAST) printf("capture stuff done\n");


    	p = deciphered.p;

		location_assign(&old, location_getfile(p->currentLocation), location_getrank(p->currentLocation));
       
    	destination = deciphered.loc;

		p->currentLocation = destination;

    	isCastle = p->type == PIECE_KING && !(p->hasMoved) && square(location_getfile(old) - location_getfile(destination)) == 4;
    	team = p->color == TEAM_WHITE ? board->White : board->Black;
    	if(isCastle)
    	{
        	rook = team[location_getfile(old) - location_getfile(destination) < 0 ? I_ROOK2 : I_ROOK1];
        	assert(!(rook->hasMoved));
			assert(location_getrank(rook->currentLocation) == 1 || location_getrank(rook->currentLocation) == 8);

        	location_setfile(&(rook->currentLocation), rook == team[I_ROOK2] ? location_getfile(p->currentLocation) - 1 : location_getfile(p->currentLocation) + 1);
    	}

    	pKing = (p->color == TEAM_WHITE ? board->White : board->Black)[I_KING];
    	if(is_valid_move(board, pKing, pKing->currentLocation, VALID_SELFCHECK | (flags & VALID_BROADCASTCALL)))
    	{
	    	char promoted, *str;


        	moved |= destination;
			moved |= old << 8;

        	p->hasMoved = true;
        	if(isCastle)
			{
				moved = 0;
            	rook->hasMoved = true;

				moved |= location_getrank(rook->currentLocation) << 8;
				moved |= rook == team[I_ROOK1] ? 0xf015: 0xf058; /* 1111 0000 0001 0101 : 1111 0000 0101 1000 */

				assert(moved == 0xf115 || moved == 0xf815 || moved == 0xf158 || moved == 0xf858);
			}

        	add_move(&(board->Moves), PGNMule);

        	if(flags & VALID_BROADCASTCALL) printf("move added\n");


        	promoted = piece_promoted(p, inStr);
        	post_PGN(PGNMule, board, promoted);

        	if(flags & VALID_BROADCASTCALL) printf("post_PGN = %s\n", PGNMule);


        	str = malloc(10 * sizeof(char));
        	if(flags & VALID_BROADCASTCALL) printf("str memory allocated\n");

        	string_copy(str, PGNMule);
        	if(flags & VALID_BROADCASTCALL) printf("string copied\n");

        	update_latest_move(&(board->Moves), str);
        	if(flags & VALID_BROADCASTCALL) printf("latest move updated\n");
    	}
    	else
    	{
        	if(flags & MOVE_BROADCAST) printf("bad move branch\n");

			p->currentLocation = old;

        	if(at != NULL) uncapture(board, at, at_index, deciphered.loc);

        	if(isCastle) location_setfile(&(rook->currentLocation), rook == team[I_ROOK2] ? 8 : 1);
    	}
    }

    if(flags & MOVE_RUNTIME)
    {
        uintmax_t endtime = clock();

        functime = ((double) (endtime - start)) / CLOCKS_PER_SEC;
    }

    if(flags & VALID_BROADCASTCALL) printf("process_move returning %s\n", moved ? "true" : "false");

    return moved;
}


void print_pieces(Game *board, int_fast8_t flags)
{
    Piece *current;
    uint_fast8_t i, j;

    for(j = 0; j < 2; j++)
    {
        printf("%s:\n-------------\n", j == 0 ? "White pieces" : "Black pieces");
        for(i = 0; i < PIECES_PER_SIDE; i++)
        {
	    char coordinate[3];

            current = j == 0 ? board->White[i] : board->Black[i];

            printf("%s\n", get_piece_name(current));

            location_to_coordinate_string(coordinate, current->currentLocation);

            printf("Location: %s\n", coordinate);

            if(flags & PP_SHOWCAPTURED) printf("Captured: %s\n", current->currentLocation == 0 ? "yes" : "no");

            printf("\n");
        }
        printf("\n");
    }
}



int_fast8_t BORDERCHAR = BLACK;
int_fast8_t BORDERTILE = YELLOW;
int_fast8_t INBETWEENCHAR = WHITE;
int_fast8_t INBETWEENTILE = GREY;
int_fast8_t WHITETILE = YELLOW;
int_fast8_t BLACKTILE = RED;
int_fast8_t WHITEPIECE = WHITE_BR;
int_fast8_t BLACKPIECE = BLACK;


/**
 * Prints the chessboard along with the movelist if the flag is chosen.
 *
 * @param board   The game instance being played on
 * @param flags   The flags for the function
 */
void print_board(Game *board, int_fast8_t flags)
{
    const char hyphens[18] = "-----------------";
    uint_fast8_t i, j;
    Location loc;
    Piece *All[2 * PIECES_PER_SIDE], *current;
    bool usable;

    char *topEndFill = "\t";
    if(flags & PB_GAMEOVER)
    {
	uint_fast8_t mask;

        assert((flags & PB_RESULTMASK) != 0);

        mask = (flags & PB_RESULTMASK) >> 2;

        assert(mask >= 1 && mask <= 3);



        switch(mask)
        {
            case 0:
                topEndFill = "CONTINUED";
                break;
            case 1:
                topEndFill = "STALEMATE";
                break;
            case 2:
                topEndFill = "WHITE WINS";
                break;
            case 3:
                topEndFill = "BLACK WINS";
        }
    }

    get_all_pieces(All, board->White, board->Black);

    makeColor(BORDERCHAR, BORDERTILE);
    printf("%s", hyphens);
    RESETCOLOR;
    if(flags & PB_SHOWMOVES) printf("\t%s\t%s%s-----", topEndFill, hyphens, hyphens);
    else if(flags & PB_GAMEOVER)
        printf("\t%s%s%s", topEndFill, flags & PB_SHOWMOVES ? hyphens : "", flags & PB_SHOWMOVES ? hyphens: "");

    printf("\n");

    for(i = 8; i >= 1; i--)
    {
        for(j = 1; j <= 8; j++)
        {
	    char c;
	    int_fast8_t tileColor, pieceColor, k;

            tileColor = u_8(j, i) <= 3 ? WHITETILE : BLACKTILE;
            pieceColor = tileColor;

            assert(i == 8 && j == 1 ? tileColor == WHITETILE : 1);

            usable = false;
            for(k = 0; k < 2 * PIECES_PER_SIDE; k++)
            {
                current = All[k];
                loc = current->currentLocation;
                if(loc != 0 && location_getfile(loc) == j && location_getrank(loc) == i)
                {
                    usable = true;
                    break;
                }

            }

            if(usable)
            {
                switch(current->type)
                {
                    case PIECE_PAWN:
                        c = 'P';
                        break;
                    case PIECE_BISHOP:
                        c = 'B';
                        break;
                    case PIECE_KNIGHT:
                        c = 'N';
                        break;
                    case PIECE_ROOK:
                        c = 'R';
                        break;
                    case PIECE_KING:
                        c = 'K';
                        break;
                    case PIECE_QUEEN:
                        c = 'Q';
                        break;
                    default:
                        c = '?';
                }
                pieceColor = current->color == TEAM_WHITE ? WHITEPIECE : BLACKPIECE;
            }
            else
                c = '#';


            makeColor(BORDERCHAR, BORDERTILE);
            printf(j == 1 ? "|" : "");
            RESETCOLOR;

            makeColor(pieceColor, tileColor);
            printf("%c", c);
            RESETCOLOR;

            if(j == 8)
            {
                makeColor(BORDERCHAR, BORDERTILE);
                printf("|");
                RESETCOLOR;
                if(flags & PB_SHOWMOVES)
                {
                    const char *MOVEFORMAT = "\t\t\t%" PRIiMAX "\t\t%s\t\t%s";

                    Turn *curMove;
                    if(board->Moves.num > 8)
                    {
						char *black;

                        curMove = get_move_number(board->Moves, board->Moves.num - (i-1));
                        black = curMove->Black == NULL ? "" : curMove->Black;
                        printf(MOVEFORMAT, curMove->number, curMove->White, black);
                    }
                    else if(get_move_number(board->Moves, 9-i) != NULL)
                    {
                        curMove = get_move_number(board->Moves, 9-i);
                        if(curMove != NULL)
                        {
                            char *black = curMove->Black == NULL ? "" : curMove->Black;
                            printf(MOVEFORMAT, curMove->number, curMove->White, black);
                        }
                    }
                }
                printf("\n");
            }
            else
            {
                makeColor(INBETWEENCHAR, INBETWEENTILE);
                printf("|");
                RESETCOLOR;
            }
        }
    }

    makeColor(BORDERCHAR, BORDERTILE);
    printf("%s", hyphens);
    RESETCOLOR;
    if(flags & PB_RUNTIME) printf("\n%.1lfms (%ld)", functime * 1000, CLOCKS_PER_SEC);
    printf("\n\n");
}
