#ifndef SEARCH_H
#define SEARCH_H

#include "misc.h"
#include "board.h"

#define QMAXDEPTH 100
#define DEFAULT_TIME 3600000
#define DEFAULT_INC 0
#define ASPIRATION_WINDOW 50

void printMoveText(MOVE move);
int compareMoves(const void *a, const void *b);
void search(BOARD_STATE *board);
int setCutoff(BOARD_STATE *board);

#endif
