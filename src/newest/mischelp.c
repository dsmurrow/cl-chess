#include "mischelp.h"

#ifdef _WIN32
#include <windows.h>

void ClearScreen()
{
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
}

void makeColor(int_fast8_t text, int_fast8_t background)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, text + (16 * background));
}

#else /* !_WIN32 */
void ClearScreen()
{
    system("clear");
}

void makeColor(int_fast8_t text, int_fast8_t background)
{
    const int_fast8_t textBright = text & BRIGHTMASK ? 60 : 0;
    const int_fast8_t textColor = (text & COLORMASK) + 30 + textBright;

    const int_fast8_t backBright = background & BRIGHTMASK ? 60 : 0;
    const int_fast8_t backColor = (background & COLORMASK) + 40 + backBright;

    printf("\033[%" PRIiFAST8 ";%" PRIiFAST8 "m", textColor, backColor);
}

#endif

uintmax_t square(intmax_t num)
{
    return num * num;
}

/**
 * Utilizes abstract algebra.
 *
 * The group U(8) is the set of numbers less than 8 that are
 * relatively prime with 8 and is closed under multiplication modulo 8.
 *
 * The Cayley table for U(8) looks as follows:
 *
 *        | 3  5  1  7
 *      --|------------      The reason I use U(8) here is because of the pattern
 *      3 | 1  7  3  5       that the Cayley table produces. Notice how 1 & 3, 5,
 *      5 | 7  1  5  3       and 7 produce distinct diagonal patterns. I use this
 *      1 | 3  5  1  7       pattern to determine what color the tiles on the chess-
 *      7 | 5  3  7  1      -board are going to be based on what comes out of this function.
 *
 * Because U(8) is an Abelian (commutative, like how 1 + 3 = 3 + 1) group,
 * a distinction doesn't need to be made for whether x is the row or column
 * of numbers in the Cayley table. For a non-Abelian group, that distinction
 * would have to be made.
 *
 * @param x  The x value of a given coordinate
 * @param y  The y value of a coordinate
 *
 * @return   The value of x and y indexed into U(8) and multiplied mod 8
 */
uint_fast8_t u_8(uint_fast8_t x, uint_fast8_t y)
{
    const uint_fast8_t group[4] = {7, 3, 5, 1}; /* The ordering is different than that of the Cayley table
                                                   to accommodate counting from 0 in the array index with
                                                   my coordinates which can only go as low as 1. */

    const uint_fast8_t horiz = group[x % 4]; /* x mod 4 because this is happening to each quadrant of the chessboard. */
    const uint_fast8_t vertic = group[(y+1) % 4]; /* y is incremented to work into the indexing of the array. Such that
                                               when x == 1 && y == 8, horiz == 1 && vertic == 1. This makes the corners line up. */

    const uint_fast8_t result = (horiz * vertic) % 8; /* The multiplication mod 8 for U(8). */

    assert(result == group[0] || result == group[1] || result == group[2] || result == group[3]); /* Redundancy, just so if something went wrong
                                                                                                     I wouldn't look everywhere before I looked at this function. */

    return result;
}

/**
 * Does a quick run-through of a tokenized move to verify that it makes some sense before a bunch of
 * effort goes into it.
 *
 * @param str  The tokenized string being analyzed
 *
 * @returns true if the string makes sense and isn't just random characters
 */
bool legal_move(char *str)
{
    int_fast8_t len, i;
    bool ret;

    len = string_getlen(str);
    ret = char_is_coord(str[len - 2]) && char_is_digit(str[len - 1]);

    i = 0;
    while(str[i] != '\0' && ret)
    {
        if(!(char_is_coord(str[i]) || char_is_digit(str[i]) || char_is_piece(str[i])))
            ret = false;
        i++;
    }

    return ret;
}

/**
 * With given color, the PGM move string, and some flags, this function
 * returns a Move object with information about what piece is being moved
 * and where it's going.
 *
 * @param board  The game instance being played
 * @param color  The color of the piece being moved
 * @param str    The string being deciphered
 * @param flags  The flags for this function
 *
 * @returns      A move object with a piece pointer and a location pointer
 */
Move decipher_move(Game *board, int_fast8_t color, char *str, int_fast8_t flags)
{
    int_fast8_t i, movelen;
    char movestr[7];
    Move ret;
    Piece **SameTeam;

    SameTeam = color == TEAM_WHITE ? board->White : board->Black;


    ret.p = NULL;

    snprintf(movestr, string_getlen(str) + 1, "%s", str);
    tokenize_move(movestr);
    movelen = string_getlen(movestr);
    if(flags & DECIPHER_BROADCAST) printf("decipher_move() called. movestr = %s, movelen = %" PRIiFAST8 ", color = %" PRIdFAST8 "\n", movestr, movelen, color);

    if((movelen == 3 || movelen == 5) && movestr[0] == 'O' && movestr[1] == '-' && movestr[2] == 'O')
    {
        bool kingside, queenside;

        kingside = movelen == 3;
        queenside = movelen == 5 && movestr[3] == '-' && movestr[4] == 'O';

        if(kingside ^ queenside)
        {
            ret.p = SameTeam[I_KING];
			
			location_setrank(&(ret.loc), location_getrank(ret.p->currentLocation));
			location_setfile(&(ret.loc), location_getfile(ret.p->currentLocation) + (kingside ? 2 : -2));
        }
    }
    else if(legal_move(movestr))
    {
		location_assign(&(ret.loc), movestr[movelen - 2] - 96, movestr[movelen - 1] - '0');

        /* Pawns */
        if(char_is_coord(movestr[0]) && (movelen == 2 || movelen == 3))
        {
            const int_fast8_t deltX = movestr[0] - movestr[movelen - 2];
            const int_fast8_t verticalInc = color == TEAM_WHITE ? 1 : -1;
            if(flags & DECIPHER_BROADCAST) printf("Hit pawn branch. deltX = %" PRIdFAST8 ". vertInc = %" PRIdFAST8 "\n", deltX, verticalInc);

            if(movelen == 2)
            {
                Location forward;
                Location forward2;

                bool foundForward = false;

		location_assign(&forward, movestr[0] - 96, movestr[1] - '0' - verticalInc);
		location_assign(&forward2, location_getfile(forward), location_getrank(forward) - verticalInc);
                for(i = I_PAWN1; !foundForward && i <= I_PAWN8; i++)
                {
                    if(SameTeam[i]->currentLocation != 0 && SameTeam[i]->currentLocation == forward)
                    {
                        foundForward = true;
                        ret.p = SameTeam[i];
                    }
                    else if(SameTeam[i]->currentLocation != 0 && SameTeam[i]->currentLocation == forward2)
                        ret.p = SameTeam[i];
                }
            }
            else if(movelen == 3 && deltX * deltX == 1)
            {

                Location origin;
                location_assign(&origin, movestr[0] - 96, movestr[movelen - 1] - '0' - verticalInc);
		if(flags & MOVE_BROADCAST) printf("movelen == 3 branch. origin = {%" PRIuFAST8 ", %" PRIuFAST8 "}\n", location_getfile(origin), location_getrank(origin));

                for(i = I_PAWN1; ret.p == NULL && i <= I_PAWN8; i++)
                {
                    if(SameTeam[i]->currentLocation != 0 && origin == SameTeam[i]->currentLocation)
                        ret.p = SameTeam[i];
                }
                if((flags & MOVE_BROADCAST) && ret.p == NULL) printf("ret.p == NULL\n");
            }
        }
        else if(movelen == 3 && char_is_piece(movestr[0])) /* 'Qg3' */
        {
            switch(movestr[0])
            {
                case 'Q':
                    ret.p = SameTeam[I_QUEEN];
                    break;
                case 'K':
                    ret.p = SameTeam[I_KING];
                    break;
                case 'B':
                    ret.p = u_8(location_getfile(ret.loc), location_getrank(ret.loc)) <= 3                ?
                            (color == TEAM_WHITE ? SameTeam[I_BISHOP2] : SameTeam[I_BISHOP1]) :
                            (color == TEAM_WHITE ? SameTeam[I_BISHOP1] : SameTeam[I_BISHOP2]) ;
                    break;
                case 'N':
                case 'R':
                {
                    bool isKnight = movestr[0] == 'N';
                    Piece *p1 = SameTeam[I_ROOK1 + isKnight];
                    Piece *p2 = SameTeam[I_ROOK2 - isKnight];

                    bool p1_canReach = is_valid_move(board, p1, ret.loc, flags & DECIPHER_BROADCAST);
                    bool p2_canReach = is_valid_move(board, p2, ret.loc, flags & DECIPHER_BROADCAST);

                    if(p1_canReach ^ p2_canReach)
                        ret.p = p1_canReach ? p1 : p2;
                    break;
                }
            }
        }
        else if(movelen == 4 && char_is_piece(movestr[0]) && (char_is_coord(movestr[1]) || char_is_digit(movestr[1]))) /* 'Rbc3' */
        {
            switch(movestr[0])
            {
                case 'Q':
                case 'K':
                case 'B':
                {
                    char changed[8];
                    snprintf(changed, string_getlen(movestr) + 1, "%s", movestr);
                    string_remove(changed, 1);


                    ret = decipher_move(board, color, changed, flags);
                    break;
                }
                case 'N':
                case 'R':
                {
                    Piece *p1 = movestr[0] == 'N' ? SameTeam[I_KNIGHT1] : SameTeam[I_ROOK1];
                    Piece *p2 = movestr[0] == 'N' ? SameTeam[I_KNIGHT2] : SameTeam[I_ROOK2];

                    bool p1_match = char_is_coord(movestr[1]) ? movestr[1] - 96 == location_getfile(p1->currentLocation) :
                                                                movestr[1] - '0' == location_getrank(p1->currentLocation);

                    bool p2_match = char_is_coord(movestr[1]) ? movestr[1] - 96 == location_getfile(p2->currentLocation) :
                                                                movestr[1] - '0' == location_getrank(p2->currentLocation);

                    if((p1_match ^ p2_match) && is_valid_move(board, p1_match ? p1 : p2, ret.loc, flags & VALID_BROADCASTCALL))
                        ret.p = p1_match ? p1 : p2;

                    else if((p1_match && p2_match) && (is_valid_move(board, p1, ret.loc, 0) ^ is_valid_move(board, p2, ret.loc, 0)))
                    {
                        char changed[8];
                        snprintf(changed, string_getlen(movestr) + 1, movestr);
                        string_remove(changed, 1);


                        ret = decipher_move(board, color, changed, flags);
                    }
                }
            }
        }
    }

    if(flags & DECIPHER_BROADCAST) printf("ret.p == NULL ? %d\n", ret.p == NULL);

    return ret;
}

/**
 * Converts a piece moving to a new location into portable game notation.
 * (https://en.wikipedia.org/wiki/Portable_Game_Notation)
 *
 * @param destStr   The string that the new PGN is going to be written to
 * @param board     The game instance being played
 * @param mover     The piece that is being moved
 * @param endPoint  The location that P is moving to
 * @param flags     Flags that tell the function if a capture has occurred, or a
 *                  check or mate, or a promotion
 */
void to_PGN(char *destStr, Game *board, Piece *mover, Location endPoint, int_fast8_t flags)
{
    const int_fast8_t longest_basic_PGN = 6;

    if(mover->type == PIECE_KING && location_getfile(mover->currentLocation) - location_getfile(endPoint) == -2 && location_getrank(mover->currentLocation) - location_getrank(endPoint) == 0)
    {
        string_copy(destStr, "O-O");
    }
    else if(mover->type == PIECE_KING && location_getfile(mover->currentLocation) - location_getfile(endPoint) == 2 && location_getrank(mover->currentLocation) - location_getrank(endPoint) == 0)
        string_copy(destStr, "O-O-O");
    else
    {
        char coords[3], captureString[2], pieceStr[2], coordSpecifier[2];
	Move test;
	uint_fast8_t oppositeColor;

        location_to_coordinate_string(coords, endPoint);


        oppositeColor = ((mover->color) % 2) + 1;


        if(piece_is_on(board, endPoint) == oppositeColor)
            string_copy(captureString, "x");
        else if(mover->type == PIECE_PAWN && square(location_getfile(mover->currentLocation) - location_getfile(endPoint)) == 1
                && is_valid_move(board, mover, endPoint, flags & MOVE_BROADCAST))
            string_copy(captureString, "x");
        else
            captureString[0] = '\0';


        if(mover->type == PIECE_PAWN && captureString[0] == '\0')
            pieceStr[0] = '\0';
        else
        {
            pieceStr[0] = get_piece_symbol(mover);
            pieceStr[1] = '\0';
        }


        if(mover->type == PIECE_PAWN || mover->type == PIECE_BISHOP || mover->type == PIECE_QUEEN || mover->type == PIECE_KING)
            coordSpecifier[0] = '\0';
        else
        {
            Piece **moverTeam = mover->color == TEAM_WHITE ? board->White : board->Black;

            Piece *p1 = moverTeam[mover->type == PIECE_KNIGHT ? I_KNIGHT1 : I_ROOK1];
            Piece *p2 = moverTeam[mover->type == PIECE_KNIGHT ? I_KNIGHT2 : I_ROOK2];

            bool p1_valid = is_valid_move(board, p1, endPoint, flags & VALID_BROADCASTCALL);
            bool p2_valid = is_valid_move(board, p2, endPoint, flags & VALID_BROADCASTCALL);

            if(p1_valid ^ p2_valid || !(p1_valid && p2_valid))
                coordSpecifier[0] = '\0';
            else
            {
                coordSpecifier[0] = location_getfile(p1->currentLocation) != location_getfile(p2->currentLocation) ? location_getfile(mover->currentLocation) + 96 :
                                                                                                      			location_getrank(mover->currentLocation) + '0'     ;
                coordSpecifier[1] = '\0';
            }
        }

        snprintf(destStr, longest_basic_PGN, "%s%s%s%s", pieceStr, coordSpecifier, captureString, coords);

        if(flags & PGN_BROADCAST) printf("destStr = %s\n", destStr);

        test = decipher_move(board, mover->color, destStr, flags & DECIPHER_BROADCAST);
        assert(test.p == mover);
        assert(endPoint == test.loc);
    }
}

/**
 * Puts the finishing touches on a PGN string after the piece has been moved and such. Adds
 * characters relating to pawn promotions and check/checkmates.
 *
 * @param destStr   Where the new characters will be appended to
 * @param board     The Game instance being played
 * @param promoChar A char indicating what a piece is being promoted to. If 0 there is no promotion
 */
void post_PGN(char *destStr, Game *board, char promoChar)
{
    char promotion[3], check[2];
    uint_fast8_t mate;


    if(promoChar != 0)
    {
        promotion[0] = '=';
        promotion[1] = promoChar;
        promotion[2] = '\0';
    }
    else
        promotion[0] = '\0';

    string_concatenate(destStr, promotion);



    mate = check_if_check(board);

    if(mate & CHECK_YES)
    {
        check[0] = mate & CHECK_MATE ? '#' : '+';
        check[1] = '\0';
    }
    else
        check[0] = '\0';

    string_concatenate(destStr, check);
}

