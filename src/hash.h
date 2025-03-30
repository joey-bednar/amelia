#ifndef HASH_H
#define HASH_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern void initZobrist();
extern void loadZobrist(BOARD_STATE *board);
extern void updateZobrist(int sq64, int piece, BOARD_STATE *board);
extern void turnZobrist(BOARD_STATE *board);
extern void updateZobristEp(int start, int end, BOARD_STATE *board);
extern void updateZobristCastle(int start, int end, BOARD_STATE *board);

#endif
