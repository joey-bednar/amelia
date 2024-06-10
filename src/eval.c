#include "defs.h"
#include <assert.h>
#include <stdio.h>

int eval(BOARD_STATE *board) {
    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    const int val[] = {1, 3, 3, 5, 9, 1000};
    for (int i = bbPawn; i < bbKing; i++) {
        int me = countBits(mine & board->bitboard[i]);
        int you = countBits(yours & board->bitboard[i]);
        total += val[i - bbPawn] * (me - you);
    }

    return total;
}

int negaMax(BOARD_STATE *board, int depth) {
    if (depth == 0) {
        return eval(board);
    }
    int max = -999;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            // printf("Start:\n");
            // printBoard(board);
            makeMove(board, moves[i]);

            // printf("End:\n");
            // printBoard(board);
            int score = -negaMax(board, depth - 1);

            if (score > max) {
                max = score;
            }

            unmakeMove(board, moves[i]);
        }
    }

    return max;
}

void printBestMove(int depth, BOARD_STATE *board) {
    int startSq = 0;
    int endSq = 0;
    int max = -10000;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            makeMove(board, moves[i]);
            int score = -negaMax(board, depth);

            unmakeMove(board, moves[i]);
            if (score > max) {
                max = score;
                startSq = moves[i].startSquare;
                endSq = moves[i].endSquare;
                // printf("negamax: %d; move: %d to %d\n", max, startSq, endSq);
            }
        }
    }

    printf("(%d): move %d to %d\n", max, startSq, endSq);
}
