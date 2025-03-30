#ifndef ATTACK_H
#define ATTACK_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern int isAttacked(BOARD_STATE *board, int sq, int enemycolor);
extern int isLegalMove(BOARD_STATE *board, MOVE move);
extern int isCheck(BOARD_STATE *board, MOVE move);

#endif
