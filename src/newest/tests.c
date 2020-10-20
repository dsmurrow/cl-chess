#include "tests.h"
#include "commands.h"

void test_valid_macros()
{
    uint_fast8_t i, j;

    uint_fast8_t is_validMacros[5] = {VALID_BROADCASTCALL, VALID_IGNORECOLOR, VALID_KINGNOLIMIT, VALID_PAWNATKONLY, VALID_SELFCHECK};
    for(i = 0; i < 5; i++)
    {
        for(j = i + 1; j < 5; j++)
        {
            assert(!(is_validMacros[i] & is_validMacros[j]));
        }
    }
}

void test_PGN_macros()
{
    assert((char)(PGN_PROMOTION_QUEEN & PGN_PROMOTIONMASK) == 'Q');
    assert((char)(PGN_PROMOTION_BISHOP & PGN_PROMOTIONMASK) == 'B');
    assert((char)(PGN_PROMOTION_KNIGHT & PGN_PROMOTIONMASK) == 'N');
    assert((char)(PGN_PROMOTION_ROOK & PGN_PROMOTIONMASK) == 'R');


}

void test_piece_macros()
{
    uint_fast8_t i, j, piecechecks[5];
    assert((PIECE_BISHOP | PIECE_ROOK) == PIECE_QUEEN);

    piecechecks[0] = PIECE_PAWN;
    piecechecks[1] = PIECE_ROOK;
    piecechecks[2] = PIECE_KNIGHT;
    piecechecks[3] = PIECE_BISHOP;
    piecechecks[4] = PIECE_KING;

    for(i = 0; i < 5; i++)
    {
        for(j = i + 1; j < 5; j++)
        {
            assert(!(piecechecks[i] & piecechecks[j]));
        }
    }
}

void test_on_board_macro()
{
    int_fast8_t i, j;

    i = INT_FAST8_MIN;
    while(i > INT_FAST8_MIN)
    {
        j = INT_FAST8_MIN;
        while(j > INT_FAST8_MIN)
        {
            if(i > 0 && i < 9 && j > 0 && j < 9)
                assert(IS_ON_BOARD(i,j));
            else
                assert(!(IS_ON_BOARD(i, j)));
            j++;
        };
        i++;
    };
}


void test_macros()
{
    assert(TEAM_WHITE != TEAM_BLACK);

    test_on_board_macro();
    test_piece_macros();
    test_valid_macros();
}




void test_piece_placement()
{
    Game *board = init_game();

    Piece **WhitePieces = board->White;
    Piece **BlackPieces = board->Black;

    uint_fast8_t i;
    Piece *curW, *curB;

    /* Initialize an array making an array to map the macros to piece types */
    uint_fast8_t pieceMap[PIECES_PER_SIDE];
    for(i = 0; i < PIECES_PER_SIDE; i++)
    {
        switch(i)
        {
            case I_PAWN1:
            case I_PAWN2:
            case I_PAWN3:
            case I_PAWN4:
            case I_PAWN5:
            case I_PAWN6:
            case I_PAWN7:
            case I_PAWN8:
                pieceMap[i] = PIECE_PAWN;
                break;
            case I_ROOK1:
            case I_ROOK2:
                pieceMap[i] = PIECE_ROOK;
                break;
            case I_KNIGHT1:
            case I_KNIGHT2:
                pieceMap[i] = PIECE_KNIGHT;
                break;
            case I_BISHOP1:
            case I_BISHOP2:
                pieceMap[i] = PIECE_BISHOP;
                break;
            case I_QUEEN:
                pieceMap[i] = PIECE_QUEEN;
                break;
            case I_KING:
                pieceMap[i] = PIECE_KING;
                break;
            default:
                pieceMap[i] = 6;
        }

    }

    /* Testing all pieces are in the right place */
    for(i = 0; i < PIECES_PER_SIDE; i++)
    {   
 	Location temp;
	uint_fast8_t x, y;

        assert(pieceMap[i] != 6);

        curW = WhitePieces[i];
        curB = BlackPieces[i];

        assert(curW->type == pieceMap[i]);
        assert(curB->type == pieceMap[i]);

        assert(curW->hasMoved == false);
        assert(curW->hasMoved == false);

        x = (i % 8) + 1;
        y = 2 - (i / 8);


        assert(location_equals_coords(curW->currentLocation, x, y));
        assert(location_equals_coords(curB->currentLocation, x, 9 - y));

	location_assign(&temp, x, y);
        assert(piece_is_on(board, temp));
    }

    free_game(board);
}

void test_castling()
{
    uint_fast8_t i;
    Location kingside, queenside;
    Game *board = init_game();

    Piece **WhitePieces = board->White;
    Piece **BlackPieces = board->Black;


    /* Remove every piece between the kings and the rooks. */
    for(i = I_KNIGHT1; i < I_ROOK2; i++)
    {
        if(WhitePieces[i]->type != PIECE_KING)
        {
            capture(board, WhitePieces[i]);
			capture(board, BlackPieces[i]);
    	}
	}

    location_assign(&kingside, 7, 1);
    assert(is_valid_move(board, WhitePieces[I_KING], kingside, 0));

    location_assign(&queenside, 3, 1);
    assert(is_valid_move(board, WhitePieces[I_KING], queenside, 0));


    location_setrank(&kingside, 8);
    assert(is_valid_move(board, BlackPieces[I_KING], kingside, 0));

    location_assign(&queenside, 3, 8);
    assert(is_valid_move(board, BlackPieces[I_KING], queenside, 0));

    free_game(board);
}




void test_pawn_forward()
{
    Game *board = init_game();

    Piece **WhitePieces = board->White;
    Piece **BlackPieces = board->Black;

    Location temp;
    uint_fast8_t i;
    for(i = I_PAWN1; i <= I_PAWN8; i++)
    {
 	Piece *w, *b;

        w = WhitePieces[i];

	location_assign(&temp, location_getfile(w->currentLocation), 3);
        assert(is_valid_move(board, w, temp, 0));

        location_setrank(&temp, location_getrank(temp) + 1);
        assert(is_valid_move(board, w, temp, 0));


        b = BlackPieces[i];

        location_setrank(&temp, 6);
        assert(is_valid_move(board, b, temp, 0));

        location_setrank(&temp, location_getrank(temp) - 1);
        assert(is_valid_move(board, b, temp, 0));
    }

    free_game(board);
}

void test_pawn_capture()
{
    Piece *w, *b;
    Game *board = init_game();

    w = board->White[I_PAWN5];
	location_setrank(&(w->currentLocation), 4);

    b = board->Black[I_PAWN4];
    location_setrank(&(b->currentLocation), 5);

    assert(is_valid_move(board, w, b->currentLocation, 0));
    assert(is_valid_move(board, b, w->currentLocation, 0));

    free_game(board);
}

void test_pawn_illegal()
{
    Game *board;
    Piece **W, **B;
    Location temp;
    uint_fast8_t i, j;

    board = init_game();

    W = board->White;
    B = board->Black;

    for(i = I_PAWN1; i <= I_PAWN8; i++)
    {
        for(j = 1; j <= 8; j++)
        {
	    location_assign(&temp, j, 0);
            assert(!is_valid_move(board, W[i], temp, 0));

            location_setrank(&temp, 8);
            assert(!is_valid_move(board, B[i], temp, 0));
        }


  	location_assign(&temp, location_getfile(W[i]->currentLocation) - 1, 3);
        assert(!is_valid_move(board, W[i], temp, 0));

        location_setfile(&temp, location_getfile(temp) + 2);
        assert(!is_valid_move(board, W[i], temp, 0));

		location_assign(&temp, location_getfile(B[i]->currentLocation) - 1, 6);
        assert(!is_valid_move(board, B[i], temp, 0));

        location_setfile(&temp, location_getfile(temp) + 2);
        assert(!is_valid_move(board, B[i], temp, 0));
    }

    free_game(board);
}




void test_decipher()
{
    Game *board = init_game();

    Move a;

    Piece **WhitePieces = board->White;
    Piece **BlackPieces = board->Black;

    /* test normal pawn movements */
    a = decipher_move(board, TEAM_WHITE, "e4", 0);
    assert(a.p == WhitePieces[I_PAWN5]);
    assert(location_getfile(a.loc) == 5);
    assert(location_getrank(a.loc) == 4);



    a = decipher_move(board, TEAM_BLACK, "b6", 0);
    assert(a.p == BlackPieces[I_PAWN2]);
    assert(location_equals_coords(a.loc, 2, 6));



    /* test castling */
    a = decipher_move(board, TEAM_WHITE, "O-O", 0);
    assert(a.p == WhitePieces[I_KING]);
    assert(location_equals_coords(a.loc, 7, 1));



    a = decipher_move(board, TEAM_BLACK, "O-O-O", 0);
    assert(a.p == BlackPieces[I_KING]);
    assert(location_equals_coords(a.loc, 3, 8));



    /* test back row moves */
    a = decipher_move(board, TEAM_WHITE, "Qxb5", 0);
    assert(a.p == WhitePieces[I_QUEEN]);
    assert(location_equals_coords(a.loc, 2, 5));




    free_game(board);
}

void test_string_remove()
{
    char egg[17] = "dog";
    char de[14] = "dog";
    string_remove(egg, 2);
    assert(egg[2] == '\0');

    string_remove(de, 0);
    assert(de[0] == 'o' && de[1] == 'g');
}

void test_string_copy()
{
    const char *ye = "ab";
    char n[5];
    string_copy(n, ye);
    assert(n[0] == 'a');
    assert(n[1] == 'b');
    assert(n[2] == '\0');
}

void test_string_count_occurences()
{
    const char *pi = "aaabbcccc";
    assert(string_count_occurences_of_char(pi, 'a') == 3);
    assert(string_count_occurences_of_char(pi, 'b') == 2);
    assert(string_count_occurences_of_char(pi, 'c') == 4);
}

void test_string_cat()
{
    char dog[11] = "dog";
    char *cat = "cat";
    string_concatenate(dog, cat);

    assert(string_matches(dog, "dogcat"));
}

void test_string_split()
{
    char empty[15][15];
    const char *egg = "true dat homie";
    uintmax_t splits = string_split(&empty[0][0], 15, egg, ' ');
    assert(splits == 3);
    assert(splits == string_count_occurences_of_char(egg, ' ') + 1);
    assert(string_matches(empty[0], "true"));
    assert(string_matches(empty[1], "dat"));
    assert(string_matches(empty[2], "homie"));
}

void test_tokenize()
{
    char a[8] = "bxc3#";
    tokenize_move(a);
    assert(a[0] == 'b' && a[1] == 'c' && a[2] == '3' && a[3] == '\0');
}

void test_move()
{
    Game *board = init_game();
    Location f3;
	
	location_assign(&f3, 6, 3);

    assert(!process_move(board, "b7", 0));

    assert(process_move(board, "b3", 0));
    assert(process_move(board, "b6", 0));

    assert(process_move(board, "Nf3", 0));
    assert(process_move(board, "Nc6", 0));

    assert(piece_is_on(board, f3));
    assert(!is_valid_move(board, board->White[5], f3, 0));

    assert(process_move(board, "Ba3", 0));
    assert(process_move(board, "f5", 0));

	location_setfile(&(board->White[I_QUEEN]->currentLocation), location_getfile(board->White[I_QUEEN]->currentLocation) + 1);
	location_setrank(&(board->White[I_QUEEN]->currentLocation), location_getrank(board->White[I_QUEEN]->currentLocation) + 2);

    assert(board->Black[5]->type == PIECE_PAWN);
    assert(!is_valid_move(board, board->Black[5], f3, 0));

    assert(process_move(board, "Qxe7", 0));
    assert(board->White[I_QUEEN]->currentLocation != 0);
    assert(board->Black[I_PAWN5]->currentLocation == 0);
    assert(process_move(board, "Qe7", 0));

    assert(process_move(board, "Bxe7", 0));
    assert(process_move(board, "Kxe7", 0));

    assert(process_move(board, "Ng5", 0));
    assert(!process_move(board, "Kf7", 0));
    assert(process_move(board, "d5", 0));

    assert(process_move(board, "e4", 0));
    assert(process_move(board, "fxe4", 0));

    assert(process_move(board, "Ne4", 0));
    assert(process_move(board, "dxe4", 0));

    free_game(board);


    board = init_game();

    assert(process_move(board, "e4", 0));
    assert(process_move(board, "d5", 0));

    assert(process_move(board, "Bd3", 0));
    assert(process_move(board, "e5", 0));

    assert(process_move(board, "Nf3", 0));
    assert(process_move(board, "Bc5", 0));

    assert(process_move(board, "O-O", 0));
    assert(process_move(board, "Bxf2", 0));

    assert(!process_move(board, "a3", 0));


    free_game(board);



    board = init_game();

    assert(process_move(board, "e4", 0));
    assert(process_move(board, "d5", 0));

    assert(process_move(board, "f4", 0));
    assert(process_move(board, "dxe4", 0));

    assert(process_move(board, "d4", 0));
    assert(process_move(board, "exd3", 0));

    free_game(board);


    board = init_game();

    assert(process_move(board, "e4", 0));
    assert(process_move(board, "a5", 0));

    assert(process_move(board, "e5", 0));
    assert(process_move(board, "d5", 0));

    assert(process_move(board, "exd6", 0));

    free_game(board);



    board = init_game();

    assert(process_move(board, "d4", 0));
    assert(process_move(board, "d5", 0));

    assert(process_move(board, "Nc3", 0));
    assert(process_move(board, "c6", 0));

    assert(process_move(board, "e4", 0));
    assert(process_move(board, "Nf6", 0));

    assert(process_move(board, "Bd3", 0));
    assert(process_move(board, "Na6", 0));

    assert(process_move(board, "Nf3", 0));
    assert(process_move(board, "Bg4", 0));

    assert(process_move(board, "O-O", 0));
    assert(process_move(board, "Qc7", 0));

    assert(process_move(board, "Re1", 0));
    assert(process_move(board, "b6", 0));

    free_game(board);


    board = init_game();

    command(board, "place 0 h7");
    assert(process_move(board, "hxg8=B+", 0));

    free_game(board);
}





void test_pawns()
{
    test_pawn_capture();
    test_pawn_forward();
    test_pawn_illegal();
}

void test_pieces()
{
    test_piece_placement();
    test_castling();

    test_pawns();
}

void test_functions()
{
    test_decipher();
    test_move();
    test_string_remove();
    test_string_count_occurences();
    test_string_split();
    test_string_cat();
    test_tokenize();
}

void testall()
{
    test_macros();
    test_pieces();
    test_functions();
}
