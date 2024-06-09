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

int main() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
        moves[i].captured = OFFBOARD;
    }
    initEnpassantMap(epMap);
    initColorMap(colorMap, notcolorMap);
    initSqMap(sq120sq64Map, sq64sq120Map, onboardMap);
    initPieceGenericMap(genericMap, toWhite, toBlack);

    clearBoard(&board);
    initBoard(&board);

    printBoardIndex(&board);
    int depth = 7;
    perft(depth);
    printBenchmark(depth);

    // int castle = 0;
    // SETBIT(castle, WK_CASTLE);
    // SETBIT(castle, WQ_CASTLE);
    // int val = CHECKBIT(castle, BQ_CASTLE);
    // printf("%d\n",castle);
    // printf("%d",val);
    // printBits(castle);

    // test();

    printBitboardIndex64();
    printBitboardIndex120();
    printBoardIndex(&board);

    return 0;
}
