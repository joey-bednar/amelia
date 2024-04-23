#include "defs.h"
#include <assert.h>
#include <stdio.h>

void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                             int index) {
    char piece = getPieceSq120(sq, board);
    char color = getColor(piece);

    char offsets[8] = {9, 10, 11, 1, -9, -10, -11, -1};

    for (int i = 0; i < sizeof(offsets); i++) {
        char squareContains = getPieceSq120(sq + offsets[i], board);
        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        if (color == WHITE) {
            assert(piece == wK);
            printf("%d\n", squareContains);
        } else {
            assert(piece == bK);
            printf("%d\n", squareContains);
        }
    }
}

void generateMoves(BOARD_STATE *board, MOVE *moves) {
    int index = 0;
    for (int file = FILE_A; file <= FILE_H; file++) {
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            char sq = FR2SQ120(file, rank);
            char piece = board->board[sq];

            if (piece == EMPTY) {
                continue;
            } else if (piece == wP || piece == bP) {
                /*generatePseudoPawnMoves(board, moves, sq, index);*/
            } else if (piece == wK || piece == bK) {
                generatePseudoKingMoves(board, moves, sq, index);
            }

            printf("Piece on %c%d\n", file + 'a', rank + 1);
        }
    }
}
