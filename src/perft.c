#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define VERBOSE FALSE
#define POSITION 0

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

#if POSITION == 0

    initBoard(&board);

#endif

#if POSITION == 3

    setPiece(wP, FILE_B, RANK_5, &board);
    setPiece(wP, FILE_E, RANK_2, &board);
    setPiece(wP, FILE_G, RANK_2, &board);
    setPiece(wR, FILE_B, RANK_4, &board);
    setPiece(wK, FILE_A, RANK_5, &board);

    setPiece(bP, FILE_C, RANK_7, &board);
    setPiece(bP, FILE_D, RANK_6, &board);
    setPiece(bP, FILE_F, RANK_4, &board);
    setPiece(bR, FILE_H, RANK_5, &board);
    setPiece(bK, FILE_H, RANK_4, &board);

#endif

    printBoard(&board);

    initEnpassantMap(epMap);

    clock_t t = clock();
    ULL num = perft_rec(depth, &board);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%lld", num);

    ULL correct[] = {1,       20,        400,        8902,       197281,
                     4865609, 119060324, 3195901860, 84998978956};
    if (depth < sizeof(correct) / sizeof(correct[0])) {
        printf("/%lld nodes ", correct[depth]);
    } else {
        printf("nodes ");
    }

    printf(" (%fs)\n", time_taken);

    return num;
}

void printBenchmark(int depth) {
    double times[] = {0.00002, 0.00003, 0.0005, 0.00736, 0.109, 2.79, 65.34};

    int length = sizeof(times) / sizeof(times[0]);
    if (depth >= length || depth < 0) {
        printf("Invalid depth.");
        return;
    }
    printf("Laptop benchmark for depth %d: (%fs)\n", depth, times[depth]);
}
