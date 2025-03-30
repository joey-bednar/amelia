#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "attack.h"
#include "board.h"
#include "moves.h"
#include "search.h"
#include "tt.h"

static ULL perftrec(int depth, BOARD_STATE *board) {
    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves, i;
    unsigned int nodes = 0;

    if (depth == 0) {
        return 1;
    }

    if (board->stopped) {
        return 0;
    }

    ULL val;
    if (probeTT(board->hash, &val, INF, -INF, depth) != TT_EMPTY) {
        return val;
    }

    n_moves = generateMoves(board, move_list);

    if (depth == 1) {
        int legal = 0;

        for (int i = 0; i < n_moves; ++i) {
            if (isLegalMove(board, move_list[i])) {
                ++legal;
            }
        }

        storeTT(board, (ULL)legal, 0, TT_EXACT_FLAG, depth);

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

    board->stopped = FALSE;

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

            if (!board->stopped) {
                printMoveText(move_list[i]);
                printf(": %llu\n", count);
            }
        }

        unmakeMove(board, move_list[i]);
    }

    t = clock() - t;
    float time_taken = ((float)t) / CLOCKS_PER_SEC; // in seconds

    // TT contains counts of positions after using perft and needs to
    // needs to be cleared before searching again.
    initTT();

    printf("\nNodes searched: %llu (%.2fs)\n", total, time_taken);
}
