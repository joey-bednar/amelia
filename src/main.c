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

    startUCI();

    // playHuman(&board);

    // int depth = 6;
    // perft(depth);
    // printBenchmark(depth);

    // loadFEN("r1b2r1k/1pq5/p2p2Q1/3n4/Pn2p3/6R1/BP4PP/3R3K w - - 1 2",
    // &board,0); for (int i = 1; i < 6; i++) {
    //     printf("Depth %d:\n",i);
    //     printBestMove(i, &board);
    // }

    // int depth = 4;
    // printBestMove(depth, &board);

    // loadFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
    //         &board);
    // printBoard(&board);

    // test();

    // printBitboardIndex64();
    // printBitboardIndex120();
    printBoardIndex();

    return 0;
}
