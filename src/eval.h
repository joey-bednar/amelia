#ifndef EVAL_H
#define EVAL_H

#ifndef DEFS_H
#include "defs.h"
#endif

#define MVVLVA(p, c) (MVVLVA[(p)][(c)])
#define PASSEDPAWN(sq64, c) (passedPawnTable[(sq64)][(c)])

extern ULL passedPawnTable[64][2];
extern ULL isolatedPawns[8];
extern int MVVLVA[8][8];

extern int pawnSqTable[2][64];
extern int knightSqTable[2][64];
extern int bishopSqTable[2][64];
extern int rookSqTable[2][64];
extern int queenSqTable[2][64];
extern int kingSqTable[2][64];

extern int isThreeFold(BOARD_STATE *board);
extern int isTwoFold(BOARD_STATE *board);
extern int isInsufficientMaterial(BOARD_STATE *board);
extern int eval(BOARD_STATE *board);
extern int isRepeatPosition(BOARD_STATE *board);
extern int getPhase(BOARD_STATE *board);

#endif
