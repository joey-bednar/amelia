#ifndef UTILS_H
#define UTILS_H

#ifndef DEFS_H
#include "defs.h"
#endif

extern void printBits(unsigned long long num);
extern void printBitboard(unsigned long long num);
extern void printBitboardIndex64();
extern void printBitboardIndex120();
extern int countBits(unsigned long long b);
extern void printBoard(BOARD_STATE *board);
extern void printBoardIndex();
extern int bitScanForward(ULL bb);
extern int bitScanReverse(ULL bb);
extern int manhattanDistance(int sq1, int sq2);

#endif
