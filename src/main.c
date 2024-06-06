#include "defs.h"
#include <assert.h>
#include <stdio.h>

int epMap[120];
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

    clearBoard(&board);
    // initBoard(&board);
    // setPiece(wK, FILE_A, RANK_1, &board);
    // setPiece(bK, FILE_H, RANK_1, &board);
    // setPiece(wP, FILE_E, RANK_2, &board);
    // setPiece(bP, FILE_D, RANK_4, &board);
    printBoard(&board);

    initEnpassantMap(epMap);
    initColorMap(colorMap, notcolorMap);

    // for (int i=0;i<3;i++) {
    //     perft(i);
    // }

    int depth = 4;
    perft(depth);
    printBenchmark(depth);

    // int num = perft_rec(2, &board);
    // printf("perft: %d\n",num);

    // test();
    printBoardIndex(&board);

    return 0;
}
