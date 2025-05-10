#ifndef INIT_H
#define INIT_H

#include "misc.h"
#include "board.h"

extern int epMap[120];

void initBoard(BOARD_STATE *board);
void clearBoard(BOARD_STATE *board);
void init(BOARD_STATE *board);
void initHistoryHeuristic(BOARD_STATE *board);

#endif
