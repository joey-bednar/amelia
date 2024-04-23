#include "defs.h"
#include <assert.h>
#include <stdio.h>

void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int file,
                             int rank, int index) {

    // two/one forward, capture left/right, en passant left,right
    char offsets[6] = {20, 10, 9, 11, 19, 21};

    char piece = getPieceFR(file, rank, board);

    char color = getColor(piece);

    // color must be valid
    assert(color == WHITE || color == BLACK);

    if (color == WHITE) {
        // on second rank
        if (rank == RANK_2) {
            // TODO: check for two moves forward
        }
        // TODO: check for one move forward
        // TODO: check for captures
        // TODO: check for en passant
    } else {
        if (rank == RANK_7) {
            // TODO: check for two moves forward
        }
        // TODO: check for one move forward
        // TODO: check for captures
        // TODO: check for en passant
    }
}

void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                             int index) {
    char piece = getPieceSq120(sq, board);
    char color = getColor(piece);

    char offsets[8] = {9, 10, 11, 1, -9, -10, -11, -1};

    for (int i = 0; i < sizeof(offsets); i++) {
        char squareContains = getPieceSq120(sq + offsets[i], board);
        if (squareContains == EMPTY || squareContains > wK) {
            printf("movable");
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
                generatePseudoPawnMoves(board, moves, file, rank, index);
            } else if (piece == wK || piece == bK) {
                generatePseudoKingMoves(board, moves, sq, index);
            }

            printf("Piece on %c%d\n", file + 'a', rank + 1);
        }
    }
}
