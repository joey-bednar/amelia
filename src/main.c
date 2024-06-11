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

    char moveString[32];
    int input = TRUE;
    while (TRUE) {

        printf("Input move: ");
        while (input) {
            scanf("%s", moveString);
            input = FALSE;
        }
        printf("Move: %s\n", moveString);

        MOVE moves[MAX_LEGAL_MOVES];
        int n_moves = generateMoves(&board, moves);

        // char moveString[4] = "e2e4";

        int file = (int)(moveString[0] - 'a');
        int rank = (int)(moveString[1] - '1');
        int start = FR2SQ120(file, rank);
        file = (int)(moveString[2] - 'a');
        rank = (int)(moveString[3] - '1');
        int end = FR2SQ120(file, rank);

        int legal = FALSE;
        for (int i = 0; i < n_moves; i++) {
            if (moves[i].startSquare == start && moves[i].endSquare == end &&
                isLegalMove(&board, moves[i])) {
                printf("Played move %s\n", moveString);
                makeMove(&board, moves[i]);
                printBoard(&board);
                printf("\n\n\n\n");
                legal = TRUE;
            }
        }

        if (!legal) {
            printf("Illegal move!\n");
        } else {
            makeBestMove(4, &board);
            printBoard(&board);
            printf("\n\n\n\n");
        }

        input = TRUE;
    }

    // int depth = 6;
    // perft(depth);
    // printBenchmark(depth);

    // loadFEN("r1b2r1k/1pq5/p2p2Q1/3n4/Pn2p3/6R1/BP4PP/3R3K w - - 1 2",
    // &board); for (int i = 1; i < 6; i++) {
    //     printf("Depth %d:\n",i);
    //     printBestMove(i, &board);
    // }

    // int depth = 4;
    // printBestMove(depth, &board);

    loadFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
            &board);
    printBoard(&board);

    // test();

    // printBitboardIndex64();
    // printBitboardIndex120();
    printBoardIndex();

    return 0;
}
