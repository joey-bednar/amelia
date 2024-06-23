#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define VERBOSE 0
#define POSITION 2
#define CUSTOMFEN "8/3k4/3P4/3K4/8/8/8/8 b - - 0 1"

ULL perft_rec_bulk(int depth, BOARD_STATE *board) {
    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves, i;
    unsigned int nodes = 0;

    n_moves = generateMoves(board, move_list);

    if (depth == 1) {
        int legal = 0;

        for (int i = 0; i < n_moves; ++i) {
            if (isLegalMove(board, move_list[i])) {
                ++legal;
            }
        }
        return (ULL)legal;
    }

    for (i = 0; i < n_moves; ++i) {

#if VERBOSE == 1
        printf("depth %d:\n", depth);
        printBoard(board);
        printf("\n");
#endif

        // printBoard(board);
        makeMove(board, move_list[i]);

#if VERBOSE == 1
        printf("depth %d:\n", depth);
        printBoard(board);
        printBitboard(board->bitboard[bbWhite]);
        printBitboard(board->bitboard[bbBlack]);
        printf("\n================\n");
#endif

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            nodes += perft_rec_bulk(depth - 1, board);
        }

        // printBoard(board);
        unmakeMove(board, move_list[i]);
        // printBoard(board);
        // assert(hash==board->hash);
    }
    return nodes;
}

ULL perft(int depth) {
    BOARD_STATE board;
    clearBoard(&board);

#if POSITION == 0

    // initBoard(&board);

    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board,
            0);

#endif

#if POSITION == 2

    loadFEN(
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        &board, 0);

#endif

#if POSITION == 3

    loadFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - 0 1", &board, 0);

#endif

#if POSITION == 99

    loadFEN(CUSTOMFEN, &board, 0);

#endif

    printf("Initial board:\n");
    printBoard(&board);

    clock_t t = clock();
    ULL num = perft_rec_bulk(depth, &board);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%lld", num);

    // position 0
    ULL correct[] = {1,       20,        400,        8902,       197281,
                     4865609, 119060324, 3195901860, 84998978956};
    // position 2
    // ULL correct[] = {48,       2039,        97862,        4085603, 193690690,
    //                  8031647685};

    // position 3
    // ULL correct[] = {14,       191,        2812,        43238,       674624,
    //                  11030083, 178633661, 3009794393};

    if (depth < sizeof(correct) / sizeof(correct[0])) {
        printf("/%lld nodes ", correct[depth]);
    } else {
        printf("nodes ");
    }

    printf(" (%fs)\n", time_taken);

    printf("Final board:\n");
    printBoard(&board);

    return num;
}
