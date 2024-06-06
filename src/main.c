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
    setPiece(wK, FILE_A, RANK_3, &board);
    // setPiece(bK, FILE_H, RANK_1, &board);
    // setPiece(wP, FILE_E, RANK_2, &board);
    // setPiece(bP, FILE_D, RANK_4, &board);
    printBoard(&board);

    ULL test = board.pieces[EMPTY];

    printBits(test);
    printf("\n");
    printBitboard(test);
    printf("\n");

    int bits = sizeof(test) * 8;
    unsigned long mask = 1UL << (bits - 1);

    for (int i = 0; i < 64; i++) {

        if (test & 1UL) {
            printf("index: %d\n", SQ64SQ120(i));
        }
        // int val = (int)((test & (0x01UL << 1)) >> 1);
        // printf("%d\n",val);
        test >>= 1;

        printBits(test);
        printf("\n");
        // printBitboard(test);
        // printf("\n");
    }

    // int depth = 6;
    // perft(depth);
    // printBenchmark(depth);

    // test();

    printBoardIndex(&board);

    return 0;
}
