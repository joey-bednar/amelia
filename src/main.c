#include "defs.h"
#include <assert.h>
#include <stdio.h>

int epMap[120];
int sq120sq64Map[120];
int sq64sq120Map[64];
int colorMap[OFFBOARD + 1];
int notcolorMap[OFFBOARD + 1];
int genericMap[bbLength];
int toWhite[bbLength];
int toBlack[bbLength];
int onboardMap[120];
ULL knightJumps[64];
ULL kingJumps[64];

int main() {
    BOARD_STATE board;

    initEnpassantMap(epMap);
    initColorMap(colorMap, notcolorMap);
    initSqMap(sq120sq64Map, sq64sq120Map, onboardMap);
    initPieceGenericMap(genericMap, toWhite, toBlack);
    initJumps(knightJumps, kingJumps);

    clearBoard(&board);
    initBoard(&board);

    printBoardIndex();
    int depth = 4;
    perft(depth);
    printBenchmark(depth);

    // test();

    // printBitboardIndex64();
    // printBitboardIndex120();
    // printBoardIndex();

    return 0;
}
