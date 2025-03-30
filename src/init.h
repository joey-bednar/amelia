#ifndef INIT_H
#define INIT_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern void initBoard(BOARD_STATE *board);
extern void clearBoard(BOARD_STATE *board);
extern void init(BOARD_STATE *board);
extern void initHistoryHeuristic(BOARD_STATE *board);

#endif
