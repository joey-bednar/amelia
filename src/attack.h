#ifndef ATTACK_H
#define ATTACK_H

#ifndef DEFS_H
#include "defs.h"
#endif

#ifndef BOARD_H
#include "board.h"
#endif

extern ULL knightJumps[64];
extern ULL kingJumps[64];
extern ULL slidingRay[8][64];

extern int isAttacked(BOARD_STATE *board, int sq, int enemycolor);
extern int isLegalMove(BOARD_STATE *board, MOVE move);
extern int isCheck(BOARD_STATE *board, MOVE move);

#endif
