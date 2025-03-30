#ifndef EVAL_H
#define EVAL_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern int isThreeFold(BOARD_STATE *board);
extern int isTwoFold(BOARD_STATE *board);
extern int isInsufficientMaterial(BOARD_STATE *board);
extern int eval(BOARD_STATE *board);
extern int isRepeatPosition(BOARD_STATE *board);
extern int getPhase(BOARD_STATE *board);

#endif
