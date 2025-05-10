#ifndef HASH_H
#define HASH_H

#include "misc.h"
#include "board.h"

extern ULL zobrist_vals[12][64];
extern ULL zobristB2M;
extern ULL zobristEP[120];
extern ULL zobristC[64];


void initZobrist();
void loadZobrist(BOARD_STATE *board);
void updateZobrist(int sq64, int piece, BOARD_STATE *board);
void turnZobrist(BOARD_STATE *board);
void updateZobristEp(int start, int end, BOARD_STATE *board);
void updateZobristCastle(int start, int end, BOARD_STATE *board);

#endif
