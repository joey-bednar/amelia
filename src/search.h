#ifndef SEARCH_H
#define SEARCH_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern void printMoveText(MOVE move);
extern int compareMoves(const void *a, const void *b);
extern void search(BOARD_STATE *board);
extern int setCutoff(BOARD_STATE *board);

#endif
