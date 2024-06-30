#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define VERBOSE 0
#define POSITION 0
#define CUSTOMFEN "8/3k4/3P4/3K4/8/8/8/8 b - - 0 1"

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

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            nodes += perftrec(depth - 1, board);
        }

        unmakeMove(board, move_list[i]);
    }
    return nodes;
}

void perft(int depth, BOARD_STATE *board) {
    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves, i;
    unsigned int nodes = 0;

    ULL total = 0;

    if (depth <= 0) {
        return;
    }

    n_moves = generateMoves(board, move_list);

    for (i = 0; i < n_moves; ++i) {

        makeMove(board, move_list[i]);

        ULL count = perftrec(depth - 1, board);
        total += count;

        printMoveText(move_list[i]);
        printf(": %llu\n", count);

        unmakeMove(board, move_list[i]);
    }

    printf("\nNodes searched: %llu\n", total);
}
