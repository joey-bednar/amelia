#ifndef MOVES_H
#define MOVES_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern int generateMoves(BOARD_STATE *board, MOVE *move);
extern void makeMove(BOARD_STATE *board, MOVE move);
extern void unmakeMove(BOARD_STATE *board, MOVE move);
extern void makeNullMove(BOARD_STATE *board);
extern void unmakeNullMove(BOARD_STATE *board);

#endif
