#include "defs.h"
#include <assert.h>
#include <stdio.h>

void generatePawnMoves(BOARD_STATE* board, MOVE* moves, int index) {}

static void findPawns(BOARD_STATE* board, MOVE* moves, int index) {
    for (int rank = RANK_1; rank <= RANK_8; rank++) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            char sq = FR2SQ(file, rank);
            char piece = board->board[sq];

            // begin pawn logic
            if (piece == wP || piece == bP) {
                generatePawnMoves(board, moves, index);
            }
        }
    }
}


void generateMoves(BOARD_STATE* board, MOVE* moves) {
    moves[0].startSquare = D2;
    moves[0].endSquare = D4;
}
