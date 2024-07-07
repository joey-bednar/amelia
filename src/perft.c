#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

static ULL perftrec(int depth, BOARD_STATE *board) {
    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves, i;
    unsigned int nodes = 0;

    n_moves = generateMoves(board, move_list);

    if (depth == 0) {
        return 1;
    }

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

        makeMove(board, move_list[i]);

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            nodes += perftrec(depth - 1, board);
        }

        unmakeMove(board, move_list[i]);
    }
    return nodes;
}

void perft(int depth, BOARD_STATE *board) {

    ULL total = 0;

    if (depth <= 0) {
        return;
    }

    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, move_list);

    clock_t t = clock();

    ULL count = 0;

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, move_list[i]);

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            count = perftrec(depth - 1, board);
            total += count;

            printMoveText(move_list[i]);
            printf(": %llu\n", count);
        }

        unmakeMove(board, move_list[i]);
    }

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    printf("\nNodes searched: %llu (%fs)\n", total, time_taken);
}
