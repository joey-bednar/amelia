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
        if (isLegalMove(board, moves[i])) {

#if VERBOSE == 1
            printf("depth %d:\n", depth);
            printBoard(board);
            printf("\n");
#endif

            makeMove(board, moves[i]);

#if VERBOSE == 1
            printf("depth %d:\n", depth);
            printBoard(board);
            printBitboard(board->pieces[EMPTY]);
            printf("\n================\n");
#endif

            nodes += perft_rec(depth - 1, board);

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

void printBenchmark(int depth) {
    double times[] = {0,0.000036,0.000352,0.00736,0.1616,4.06,66.97};

    int length = sizeof(times)/sizeof(times[0]);
    if (depth >= length || depth < 0) {
        printf("Invalid depth.");
        return;
    }
    printf("Laptop benchmark for depth %d: (%fs)\n",depth,times[depth]);
}
