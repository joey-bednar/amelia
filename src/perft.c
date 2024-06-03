#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define VERBOSE FALSE

ULL perft_rec(int depth, BOARD_STATE *board) {
    int n_moves, i;
    ULL nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    MOVE moves[MAX_LEGAL_MOVES];

    n_moves = generateMoves(board, moves);

    for (i = 0; i < n_moves; i++) {
        // printf("start:\n");
        // printBoard(board);
        // printf("\n");
        if (isLegalMove(board, moves[i])) {

            if (VERBOSE) {
                printf("depth %d:\n", depth);
                printBoard(board);
                printf("\n");
            }

            makeMove(board, moves[i]);

            if (VERBOSE) {
                printf("depth %d:\n", depth);
                printBoard(board);
                printf("\n================\n");
            }

            nodes += perft_rec(depth - 1, board);

            // printf("unmake:\n");
            // printBoard(board);
            // printf("\n");
            unmakeMove(board, moves[i]);
        }
    }

    return nodes;
}

ULL perft(int depth) {
    BOARD_STATE board;
    board.turn = WHITE;
    board.castle = 0b1111;
    board.enpassant = OFFBOARD;
    board.fiftyMove = 0;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
        moves[i].captured = OFFBOARD;
    }

    clearBoard(&board);
    initBoard(&board);
    initEnpassantMap(epMap);

    clock_t t = clock();
    ULL num = perft_rec(depth, &board);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%lld nodes (%fs)\n", num, time_taken);

    return num;
}
