#include "commands.h"
#include "logichelp.h"
#include "mischelp.h"
#include "strings.h"



extern int_fast8_t BORDERCHAR;
extern int_fast8_t BORDERTILE;

extern int_fast8_t INBETWEENCHAR;
extern int_fast8_t INBETWEENTILE;

extern int_fast8_t WHITETILE;
extern int_fast8_t BLACKTILE;

extern int_fast8_t WHITEPIECE;
extern int_fast8_t BLACKPIECE;

static void changecolor(char *type, char *color)
{
    int_fast8_t *var, colorarg;
    var = NULL;

    if(string_matches(type, "borderchar") || string_matches(type, "bc"))
        var = &BORDERCHAR;
    else if(string_matches(type, "bordertile") || string_matches(type, "bordertiles"))
        var = &BORDERTILE;
    else if(string_matches(type, "inbetweenchar") || string_matches(type, "ibc") || string_matches(type, "inbchar"))
        var = &INBETWEENCHAR;
    else if(string_matches(type, "inbetweentile") || string_matches(type, "ibt") || string_matches(type, "inbtile"))
        var = &INBETWEENTILE;
    else if(string_matches(type, "whitetile") || string_matches(type, "whitetiles") || string_matches(type, "wt"))
        var = &WHITETILE;
    else if(string_matches(type, "blacktile") || string_matches(type, "blacktiles") || string_matches(type, "bt"))
        var = &BLACKTILE;
    else if(string_matches(type, "whitepiece") || string_matches(type, "whitepieces") || string_matches(type, "wp"))
        var = &WHITEPIECE;
    else if(string_matches(type, "blackpiece") || string_matches(type, "blackpieces") || string_matches(type, "bp"))
        var = &BLACKPIECE;

    if(var == NULL) return;


    colorarg = -1;

    if(string_matches(color, "white_br") || string_matches(color, "w_br") || string_matches(color, "wbr"))
        colorarg = WHITE_BR;
    else if(string_matches(color, "black"))
        colorarg = BLACK;
    else if(string_matches(color, "grey") || string_matches(color, "gray"))
        colorarg = GREY;
    else if(string_matches(color, "red_br") || string_matches(color, "rbr") || string_matches(color, "r_br"))
        colorarg = RED_BR;
    else if(string_matches(color, "pink_br") || string_matches(color, "pbr") || string_matches(color, "p_br"))
        colorarg = PINK_BR;
    else if(string_matches(color, "magenta_br") || string_matches(color, "mbr") || string_matches(color, "m_br"))
        colorarg = MAGENTA_BR;
    else if(string_matches(color, "blue_br") || string_matches(color, "bbr") || string_matches(color, "b_br"))
        colorarg = BLUE_BR;
    else if(string_matches(color, "cyan_br") || string_matches(color, "cbr") || string_matches(color, "c_br"))
        colorarg = CYAN_BR;
    else if(string_matches(color, "green_br") || string_matches(color, "gbr") || string_matches(color, "g_br"))
        colorarg = GREEN_BR;
    else if(string_matches(color, "yellow_br") || string_matches(color, "ybr") || string_matches(color, "y_br"))
        colorarg = YELLOW_BR;
    else
    {
        switch(color[0])
        {
            case 'w':
                colorarg = WHITE;
                break;
            case 'r':
                colorarg = RED;
                break;
            case 'p':
                colorarg = PINK;
                break;
            case 'm':
                colorarg = MAGENTA;
                break;
            case 'b':
                colorarg = BLUE;
                break;
            case 'c':
                colorarg = CYAN;
                break;
            case 'g':
                colorarg = GREEN;
                break;
            case 'y':
                colorarg = YELLOW;
                break;
        }
    }


    if(colorarg == -1) return;


    *var = colorarg;
}


static void placepiece(Game *board, char *i, char *cord)
{
    uint_fast8_t index;
    Location loc;

	location_assign(&loc, cord[0] - 96, cord[1] - '0');

    index = atoi(i);
    if(index < 32 && IS_ON_BOARD(location_getfile(loc), location_getrank(loc)))
    {
        int_fast8_t isOn;
        Piece **arr;

        isOn = piece_is_on(board, loc);
        if(isOn)
        {
            int_fast8_t i, j;
            Piece *piece;
            bool match;

            match = false;
            for(i = 0; !match && i < 2 * PIECES_PER_SIDE; i++)
            {
                j = i % 16;
                arr = i > 15 ? board->Black : board->White;
                if(loc == arr[j]->currentLocation)
                {
                    match = true;
                    piece = arr[j];
                }
            }
            capture(board, piece);
        }

        arr = index < 16 ? board->White : board->Black;
		arr[index % 16]->currentLocation = loc;
    }
}

void command(Game* board, const char *str)
{
    char tokens[5][20];
    int_fast8_t tokenslen = string_split(&tokens[0][0], 20, str, ' ');


    if(tokenslen == 3 && string_matches(tokens[0], "changecolor"))
        changecolor(tokens[1], tokens[2]);

    else if(tokenslen == 3 && string_matches(tokens[0], "place"))
        placepiece(board, tokens[1], tokens[2]);
}
