#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#ifdef __WIN32
    #define     IS_WIN              1

    /* COLOR REFERENCES FOR WINDOWS */
    #define     BLACK               0x0
    #define     BLUE                0x1
    #define     GREEN               0x2
    #define     CYAN                0x3
    #define     RED                 0x4
    #define     PINK                0x5
    #define     YELLOW              0x6
    #define     WHITE               0x7
    #define     GREY                0x8
    #define     BLUE_BR             0x9
    #define     GREEN_BR            0xA
    #define     CYAN_BR             0xB
    #define     RED_BR              0xC
    #define     PINK_BR             0xD
    #define     YELLOW_BR           0xE
    #define     WHITE_BR            0xF

    /* compatibility */
    #define     BLACK_BR            GREY
    #define     MAGENTA             PINK
    #define     MAGENTA_BR          PINK_BR
#else
    #define     IS_WIN              0

    /* Color references for unix/mac */
    #define     COLORMASK           0x7    /* 0111 */
    #define     BLACK               0x0    /* 0000 */
    #define     RED                 0x1    /* 0001 */
    #define     GREEN               0x2    /* 0010 */
    #define     YELLOW              0x3    /* 0011 */
    #define     BLUE                0x4    /* 0100 */
    #define     MAGENTA             0x5    /* 0101 */
    #define     CYAN                0x6    /* 0110 */
    #define     WHITE               0x7    /* 0111 */

    #define     BRIGHTMASK          0x8    /* 1000 */
    #define     BLACK_BR            BLACK   | BRIGHTMASK
    #define     RED_BR              RED     | BRIGHTMASK
    #define     GREEN_BR            GREEN   | BRIGHTMASK
    #define     YELLOW_BR           YELLOW  | BRIGHTMASK
    #define     BLUE_BR             BLUE    | BRIGHTMASK
    #define     MAGENTA_BR          MAGENTA | BRIGHTMASK
    #define     CYAN_BR             CYAN    | BRIGHTMASK
    #define     WHITE_BR            WHITE   | BRIGHTMASK

    /* compatibility stuff */
    #define     GREY                BLACK_BR
    #define     PINK                MAGENTA
    #define     PINK_BR             MAGENTA_BR
#endif /* __WIN32 */





#define         TEAM_WHITE             1
#define         TEAM_BLACK             2

#define         PIECES_PER_SIDE         16

#define			LOCATION_FILE			0xf0
#define			LOCATION_RANK			0xf

#define         PIECE_PAWN              0
#define         PIECE_BISHOP            1
#define         PIECE_KNIGHT            4
#define         PIECE_ROOK              2
#define         PIECE_KING              8
#define         PIECE_QUEEN             3

#define         IS_ON_BOARD(h, v)       v > 0 && v < 9 && h > 0 && h < 9

#define         CHECK_NO                0x0
#define         CHECK_YES               0x80
#define         CHECK_MATE              0x40

#define         DECIPHER_BROADCAST      0x1

#define         ML_PRINT                0x1
#define         ML_SHOWMOVES            0x2
#define         ML_RESET                -1
#define         ML_QUIT                 0x4
#define         ML_CLEAR                0x10
#define         ML_SHOWRUNTIME          0x20

#define         MOVE_BROADCAST          0x1
#define         MOVE_RUNTIME            0x20

#define         PB_SHOWMOVES            0x2   /* 00 0010 */
#define         PB_GAMEOVER             0x10  /* 01 0000 */
#define         PB_RESULTMASK           0xc   /* 00 1100 */
#define         PB_CONTINUED            0x10  /* 01 0000 */
#define         PB_STALEMATE            0x14  /* 01 0100 */
#define         PB_WHITEWIN             0x18  /* 01 1000 */
#define         PB_BLACKWIN             0x1c  /* 01 1100 */
#define         PB_RUNTIME              0x20  /* 10 0000 */

#define         PGN_BROADCAST           0x1
#define         PGN_PROMOTIONMASK       0x06    /* 0000 0110 */
#define         PGN_PROMOTION           0x08    /* 0000 1000 */
#define         PGN_PROMOTION_QUEEN     0x00    /* 0000 0000 */
#define         PGN_PROMOTION_BISHOP    0x02    /* 0000 0010 */
#define         PGN_PROMOTION_KNIGHT    0x04    /* 0000 0100 */
#define         PGN_PROMOTION_ROOK      0x06    /* 0000 0110 */
#define         PGN_CAPTURED            0x10    /* 0001 0000 */
#define         PGN_ISCASTLE            0x20    /* 0010 0000 */
#define         PGN_CASTLEMASK          0x02    /* 0000 0010      If the mask returns 0, it's kingside. Otherwise it's queenside */
#define         PGN_CASTLE_KINGSIDE     0x20    /* 0010 0000 */
#define         PGN_CASTLE_QUEENSIDE    0x22    /* 0010 0010 */
#define         PGN_ISCHECK             0x80    /* 1000 0000 */
#define         PGN_ISMATE              0x40    /* 0100 0000 */


#define         PP_SHOWCAPTURED 0x1

#define         VALID_IGNORECOLOR       0x4   /* Doesn't take the color of the pieces into account, thus treating every space as vacant. Used for recursion */
#define         VALID_PAWNATKONLY       0x8   /* Will only take the pawn's diagonal attack into account, not the forward move */
#define         VALID_KINGNOLIMIT       0x10  /* Will ignore the king's limit on traveling distance. Used to check castling */
#define         VALID_BROADCASTCALL     0x1   /* Does printf's throughout the method */
#define         VALID_SELFCHECK         0x40


#define 		ICURSE_SHOWMOVES		0x2



#define         I_PAWN1                 0
#define         I_PAWN2                 1
#define         I_PAWN3                 2
#define         I_PAWN4                 3
#define         I_PAWN5                 4
#define         I_PAWN6                 5
#define         I_PAWN7                 6
#define         I_PAWN8                 7
#define         I_ROOK1                 8
#define         I_KNIGHT1               9
#define         I_BISHOP1               10
#define         I_QUEEN                 11
#define         I_KING                  12
#define         I_BISHOP2               13
#define         I_KNIGHT2               14
#define         I_ROOK2                 15


#endif /* MACROS_H_INCLUDED */
