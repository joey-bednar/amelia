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

    // printBoardIndex();
    // int depth = 5;
    // perft(depth);
    // printBenchmark(depth);

    // clearBoard(&board);
    // setPiece120(wK, E1, &board);
    // setPiece120(bK, E8, &board);
    // setPiece120(bR, A8, &board);
    // setPiece120(wQ, A5, &board);

    int startSq = 0;
    int endSq = 0;
    int max = -10000;
    int depth = 5;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(&board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(&board, moves[i])) {
            printf("%d to %d\n", moves[i].startSquare, moves[i].endSquare);
            makeMove(&board, moves[i]);
            printBoard(&board);
            int score = -negaMax(&board, depth);
            printf("negamax: %d; move: %d to %d\n", max, startSq, endSq);

            unmakeMove(&board, moves[i]);
            if (score > max) {
                max = score;
                startSq = moves[i].startSquare;
                endSq = moves[i].endSquare;
            }
        }
    }

    printBoard(&board);

    printf("final negamax: %d; move: %d to %d\n", max, startSq, endSq);
    int e = eval(&board);
    printf("eval: %d\n", e);
    // test();

    // printBitboardIndex64();
    // printBitboardIndex120();
    printBoardIndex();

    return 0;
}
