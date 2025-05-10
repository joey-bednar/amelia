#ifndef MOVES_H
#define MOVES_H

#ifndef DEFS_H
#include "defs.h"
#endif

#ifndef BOARD_H
#include "board.h"
#endif

// piece offsets and promotions
#define PAWNOFFSET(c, i) (pawnOffset[(c)][(i)])
#define ROOKOFFSET(i) (rookOffset[(i)])
#define ROOKOFFSETS rookOffset
#define BISHOPOFFSET(i) (bishopOffset[(i)])
#define BISHOPOFFSETS bishopOffset
#define PROMOTES promoteTo


// move bitmasks
#define START(move) (int)((move) & 0x3Ful)
#define START120(move) SQ64SQ120(START(move))
#define END(move) (int)(((move) & 0x0FC0) >> 6)
#define END120(move) SQ64SQ120(END(move))
#define CAPTURED(move) (int)(((move) & 0x07000ul) >> 12)
#define PROMOTED(move) (int)(((move) & 0x38000ul) >> 15)
#define EPFLAG(move) (int)(((move) & 0x40000ul) >> 18)
#define TWOPAWNFLAG(move) (int)(((move) & 0x80000ul) >> 19)
#define CASTLEFLAG(move) (int)(((move) & 0x100000ul) >> 20)
#define PIECE(move) (int)(((move) & 0x00E00000ul) >> 21)


extern int pawnOffset[2][4];
extern const int rookOffset[4];
extern const int bishopOffset[4];
extern const int promoteTo[4];

extern int generateMoves(BOARD_STATE *board, MOVE *move);
extern void makeMove(BOARD_STATE *board, MOVE move);
extern void unmakeMove(BOARD_STATE *board, MOVE move);
extern void makeNullMove(BOARD_STATE *board);
extern void unmakeNullMove(BOARD_STATE *board);

#endif
