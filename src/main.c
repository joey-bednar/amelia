#include "defs.h"
#include <assert.h>
#include <stdio.h>

int epMap[120];
int sq120sq64Map[120];
int sq64sq120Map[64];
int colorMap[OFFBOARD + 1];
int notcolorMap[OFFBOARD + 1];

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
    initSqMap(sq120sq64Map, sq64sq120Map);

    clearBoard(&board);
    // initBoard(&board);
    // setPiece(wK, FILE_H, RANK_8, &board);
    // setPiece(bK, FILE_H, RANK_1, &board);
    // setPiece(wP, FILE_E, RANK_2, &board);
    // setPiece(bP, FILE_D, RANK_4, &board);
    // printBoard(&board);

    initBoard(&board);

    // ULL bb = board.pieces[EMPTY];

    // while(bb != 0) {
    //     int index64 = bitScanForward(bb);
    //     int sq = SQ64SQ120(index64);
    //
    //     ULL mask = (~1ULL << (index64));
    //     bb &= mask;
    // }

    int depth = 5;
    perft(depth);
    printBenchmark(depth);

    // test();

    printBoardIndex(&board);

    return 0;
}
