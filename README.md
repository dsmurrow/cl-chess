# cl-chess
A cute little chess game that runs on the command line

## Compilation
For my personal testing I use gcc, I don't expect clang or any other commonly-used compiler to behave differently. If you're using a compiler other than gcc then I would suggest changing the *CC* variable in *src/newest/makefile* to accomodate your compiler of choice.

This project has been made compilable with the *-std=c90* flag in gcc so this project should compile no matter how old your compiler is.

I started this project having 8-bits in mind. Even though a C project probably isn't compatible with any 8-bit machine, I enjoyed the limitation and I feel it made me more inventive in my solutions to problems. However, while working on features that aren't on here yet, I decided that I needed to expand the integer size in some areas. In the future I'll probably have a branch that uses exclusively 8-bit ints but for now that isn't completely the case (16-bit ints show up 3 times in the project currently).

## Gameplay
Upon starting the executable you're met with a rudimentary chessboard, an empty move list, and a blinking command line. This game utilizes [portable game notation](https://en.wikipedia.org/wiki/Portable_Game_Notation) and should accept any move string compatible with PGN (and then some!)

You must use an 'x' in your move string when capturing, e.g. "exd5" for a pawn capture, "Qxd7" for a queen capture, etc. Castling is done by typing "O-O" for a king-side castle and "O-O-O" for a queen-side castle. As of right now, the program won't automatically make a move a capture if your piece will land on an enemy piece (even though I'm certain I implemented that months ago) so you *must* specify that you want to perform a capture.
