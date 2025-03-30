#ifndef TT_H
#define TT_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern void initTT();
extern int probeTT(ULL hash, MOVE *best, int alpha, int beta, int depth);
extern void storeTT(BOARD_STATE *board, MOVE best, int val, int flag,
                    int depth);

#endif
