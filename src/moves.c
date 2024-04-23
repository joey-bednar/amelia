#include "defs.h"
#include <assert.h>
#include <stdio.h>

// generate all legal moves using array of offsets
// move from sq to sq+offset[i]. If square is empty or enemy piece, add to moves
// list. Used for kings and knights
static void generateSimpleMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                int *index, char *offsets) {
    char piece = getPieceSq120(sq, board);
    char color = getColor(piece);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        char nextSq = sq + offsets[i];
        char squareContains = getPieceSq120(nextSq, board);
        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        if (color == WHITE) {
            if (squareContains == EMPTY || getColor(squareContains) == BLACK) {
                printf("Move to sq %d\n", nextSq);
                moves[*index].startSquare = sq;
                moves[*index].endSquare = nextSq;
                (*index)++;
            } else {
                printf("Can't capture own piece %d on sq %d\n", squareContains,
                       nextSq);
            }
        } else {
            if (squareContains == EMPTY || getColor(squareContains) == WHITE) {
                printf("Move to sq %d\n", nextSq);
            } else {
                printf("Can't capture own piece %d on sq %d\n", squareContains,
                       nextSq);
            }
        }
    }
}

static void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    char offsets[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    generateSimpleMoves(board, moves, sq, index, offsets);
}

static void generatePseudoKnightMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    char offsets[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    generateSimpleMoves(board, moves, sq, index, offsets);
}

void generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;
    for (int file = FILE_A; file <= FILE_H; file++) {
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            char sq = FR2SQ120(file, rank);
            char piece = board->board[sq];

            if (piece == EMPTY) {
                /*printf("Empty on %c%d\n", file + 'a', rank+1);*/
            } else if (piece == wK || piece == bK) {
                printf("King on %c%d\n", file + 'a', rank + 1);
                generatePseudoKingMoves(board, moves, sq, &index);
            } else if (piece == wN || piece == bN) {
                printf("Knight on %c%d\n", file + 'a', rank + 1);
                generatePseudoKnightMoves(board, moves, sq, &index);
            }
        }
    }

    printf("%d\n", index);

    for (int i = 0; i < index; i++) {
        printf("From %d to %d\n", moves[i].startSquare, moves[i].endSquare);
    }
}
