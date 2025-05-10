#ifndef HASH_H
#define HASH_H

#ifndef DEFS_H
#include "defs.h"
#endif

#ifndef BOARD_H
#include "board.h"
#endif

extern ULL zobrist_vals[12][64];
extern ULL zobristB2M;
extern ULL zobristEP[120];
extern ULL zobristC[64];


extern void initZobrist();
extern void loadZobrist(BOARD_STATE *board);
extern void updateZobrist(int sq64, int piece, BOARD_STATE *board);
extern void turnZobrist(BOARD_STATE *board);
extern void updateZobristEp(int start, int end, BOARD_STATE *board);
extern void updateZobristCastle(int start, int end, BOARD_STATE *board);

#endif
