#include "../all/chess.h"
#include "../all/commands.h"
#include "../all/filereading.h"
#include "../all/mischelp.h"
#include "../all/logichelp.h"
#include "../all/tests.h"

typedef struct
{
    int_fast8_t flags;
    bool do_tests;
    char *filename;
} clargs_t;

clargs_t process_clargs(int, char*[]);
void play(clargs_t);
int_fast8_t mainloop(Game*, clargs_t);



int main(int argc, char *argv[])
{
    play(process_clargs(argc, argv));



    return 0;
}


clargs_t process_clargs(int argc, char *argv[])
{
    clargs_t ret;

    ret.filename = NULL;
    ret.flags = ML_SHOWMOVES | ML_CLEAR;
    ret.do_tests = false;

    if(argc > 1)
    {
        uint_fast8_t i;
        for(i = 1; i < argc; i++)
        {
            if(string_matches(argv[i], "-broadcast"))
                ret.flags |= ML_PRINT;
            else if(string_matches(argv[i], "-runtime"))
                ret.flags |= ML_SHOWRUNTIME;
            else if(string_matches(argv[i], "-test"))
                ret.do_tests = true;
            else if(string_matches(argv[i], "-nomoves"))
                ret.flags &= ~PB_SHOWMOVES;
            else if(string_matches(argv[i], "-noclear"))
                ret.flags &= ~ML_CLEAR;
            else if(string_matches(argv[i], "-open"))
            {
                bool NO_FILE_NAME_PROVIDED;

                i++;
                NO_FILE_NAME_PROVIDED = i < argc;
                assert(NO_FILE_NAME_PROVIDED);

                ret.filename = argv[i];
            }
        }
    }

    return ret;
}

void play(clargs_t args)
{
    Game *G;
    int_fast8_t result;

    if(args.do_tests) testall();

    G = NULL;
    result = -1;
    while(result == -1)
    {
        if(G != NULL) free_game(G);
        G = init_game();
        result = mainloop(G, args);
    }

    if(result != ML_QUIT)
    {
        int_fast8_t resultflag, pbflag;
        char dummy[3];

        switch(result)
        {
            case PB_STALEMATE:
            case PB_CONTINUED:
                resultflag = result;
                break;
            case TEAM_WHITE:
                resultflag = PB_WHITEWIN;
                break;
            case TEAM_BLACK:
                resultflag = PB_BLACKWIN;
                break;
        }

        pbflag = resultflag | (args.flags & (PB_SHOWMOVES | PB_RUNTIME));
        if(args.flags & ML_CLEAR) ClearScreen();
        print_board(G, pbflag);

        free_game(G);

        fgets(dummy, 3, stdin);
    }
}


int_fast8_t mainloop(Game *board, clargs_t clargenborgen)
{
    bool readingFile, game_end;
    char **filemoves;
    int_fast8_t flags, stalemate, checkmate;
    uintmax_t filemoves_size, k;


    readingFile = clargenborgen.filename != NULL;
    filemoves = NULL;
    k = 0;

    if(readingFile)
    {
        filemoves = get_moves_from_file(clargenborgen.filename, &filemoves_size);

        /*
        uintmax_t i = 0;
        do
        {
            printf("%s, ", filemoves[i]);
        } while(++i < filemoves_size);
        */

        assert(string_matches_end(filemoves[filemoves_size - 1]));
    }



    flags = clargenborgen.flags;

    game_end = false;
    stalemate = 0;
    checkmate = 0;
    while(!game_end)
    {

        bool moved = false;
        do
        {
            char input[100], *userinput;


            if(flags & ML_CLEAR) ClearScreen();
            print_board(board, (flags & (PB_SHOWMOVES | PB_RUNTIME)));

            fgets(input, 100, stdin);
			input[99] = char_array_contains(input, 100, '\0') ? '\0' : input[99];
            string_remove(input, string_getlen(input) - 1); /* remove the newline character at the end of fgets */
            userinput = !readingFile ? input : filemoves[k++];


            if(readingFile && string_matches_end(userinput))
            {
                char *adding;
                if(string_matches(userinput, "*"))
                    stalemate = PB_CONTINUED;
                else if(string_matches(userinput, "1/2-1/2"))
                    stalemate = PB_STALEMATE;

                adding = malloc((string_getlen(userinput) + 1) * sizeof(char));
                string_copy(adding, userinput);
                add_move(&(board->Moves), adding);
                break;
            }
            else if(userinput[0] != '*' && userinput[1] != '\0')
                moved = process_move(board, userinput, (flags & MOVE_BROADCAST) | (flags & MOVE_RUNTIME));
            else
            {
                userinput = userinput[1] != ' ' ? userinput + 1 : userinput + 2;
                string_tolower(userinput);

                if(string_matches(userinput, "quit"))
                {
                    checkmate = ML_QUIT;
                }
                else if(string_matches(userinput, "reset"))
                {
                    checkmate = ML_RESET;
                }
                else
                    command(board, userinput);

                break;
            }
        }
        while(!moved);

        if(moved)
        {
            int_fast8_t whosturnnext, lastmovelen;
            char *lastmove;


            whosturnnext = board->Moves.LatestMove->Black != NULL ? TEAM_WHITE : TEAM_BLACK;
            lastmove = whosturnnext == TEAM_WHITE ? board->Moves.LatestMove->Black : board->Moves.LatestMove->White;
            if(flags & ML_PRINT) printf("lastmove = %s\n", lastmove);

            lastmovelen = string_getlen(lastmove);
            if(lastmove[lastmovelen - 1] == '#')
            {
                checkmate = whosturnnext == TEAM_WHITE ? TEAM_BLACK : TEAM_WHITE;
            }
            else
            {
                /* check if stalemate */
                uint_fast8_t i, x, y;
                Location loc;
                Piece **Pieces;

                Pieces = whosturnnext == TEAM_WHITE ? board->White : board->Black;
                stalemate = PB_STALEMATE;
                for(i = 0; stalemate && i < PIECES_PER_SIDE; i++)
                {
                    for(x = 1; stalemate && x <= 8; x++)
                    {
                        for(y = 1; stalemate && y <= 8; y++)
                        {
							location_assign(&loc, x, y);
                           
                            if(Pieces[i]->currentLocation != 0 && !(location_equals_coords(Pieces[i]->currentLocation, x, y)) &&
                               is_valid_move(board, Pieces[i], loc, 0))
                               stalemate = 0;
                        }
                    }
                }
                if(flags & ML_PRINT) printf("stalemate = %" PRIdFAST8 "\n", stalemate);
            }
        }
        game_end = stalemate || checkmate;
        if(flags & ML_PRINT) printf("game_end = %d\n", game_end);
    }


    if(readingFile)
    {
        uintmax_t i;
        if(checkmate)
        {
            char *strToCopy, *mallocd;

            strToCopy = checkmate == TEAM_WHITE ? "1-0" : "0-1";

            mallocd = malloc(4 * sizeof(char));
            string_copy(mallocd, strToCopy);

            add_move(&(board->Moves), mallocd);
        }

        for(i = 0; i < filemoves_size; i++)
            free(filemoves[i]);
        free(filemoves);
    }

    return stalemate | checkmate;
}
