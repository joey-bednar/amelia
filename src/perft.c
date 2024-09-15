#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static ULL perftrec(int depth, BOARD_STATE *board) {
    MOVE move_list[MAX_LEGAL_MOVES];
    int n_moves, i;
    unsigned int nodes = 0;

    if (depth == 0) {
        return 1;
    }

    // if(board->hash == 8254161418876349170) {
    //     printBoard(board);
    // }

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

        // if (val != -1 && val != legal) {
        //     printBoard(board);
        //     printf("\nval vs legal: %llu %llu\n", val, (ULL)legal);
        //     BOARD_STATE bcopy;
        //     memcpy(&bcopy, board, sizeof(BOARD_STATE));
        //     bcopy.hash = 0ull;
        //     loadZobrist(&bcopy);
        //     printf("copy: %llu\norig: %llu\n", bcopy.hash, board->hash);
        // }

        storeTT(board->hash, (ULL)legal, 0, TT_EXACT_FLAG, depth);

        // if (board->hash == 14740458848981706522) {
        //     printf("here\n");
        //     printBoard(board);
        //     BOARD_STATE bcopy;
        //     memcpy(&bcopy, board, sizeof(BOARD_STATE));
        //     bcopy.hash = 0ull;
        //     loadZobrist(&bcopy);
        //     printf("copy: %llu\norig: %llu\n",bcopy.hash,board->hash);
        //
        // }
        // assert(val == -1 || val == legal);

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

    // if (val != -1 && val != nodes) {
    //     printBoard(board);
    //     printf("\nval vs moves: %llu %llu\n", val, (ULL)nodes);
    // }
    // assert(val == -1 || val == nodes);

    return nodes;
}

void perft(int depth, BOARD_STATE *board) {

    // printBoard(board);

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

    // printBoard(board);

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    printf("\nNodes searched: %llu (%fs)\n", total, time_taken);
}
