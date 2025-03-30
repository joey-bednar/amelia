#ifndef UCI_H
#define UCI_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern int loadFEN(char *fen, BOARD_STATE *board, int startIndex);
extern void startUCI();

#endif
