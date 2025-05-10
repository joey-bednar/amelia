#ifndef UCI_H
#define UCI_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern int inputDepth;
extern int inputMovetime;
extern int inputTime[2];
extern int inputInc[2];

extern int loadFEN(char *fen, BOARD_STATE *board, int startIndex);
extern void startUCI();

#endif
