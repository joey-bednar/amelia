#include "defs.h"
#include <assert.h>
#include <stdio.h>

// add a move to the provided moves array
static void addMove(BOARD_STATE *board, MOVE *moves, int start, int end,
                    int *index) {
    printf("Move to sq %d\n", end);
    moves[*index].startSquare = start;
    moves[*index].endSquare = end;
    moves[*index].captured = getPieceSq120(end, board);
    (*index)++;
}

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
                addMove(board, moves, sq, nextSq, index);
            }
        } else {
            if (squareContains == EMPTY || getColor(squareContains) == WHITE) {
                printf("Move to sq %d\n", nextSq);
                addMove(board, moves, sq, nextSq, index);
            }
        }
    }
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, char *offsets) {

    char piece = getPieceSq120(sq, board);
    char color = getColor(piece);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        char nextSq = sq + offsets[i];
        char squareContains = getPieceSq120(nextSq, board);

        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move
        if (color == WHITE) {

            while (squareContains == EMPTY) {
                addMove(board, moves, sq, nextSq, index);

                nextSq = nextSq + offsets[i];
                squareContains = getPieceSq120(nextSq, board);
            }
            if (getColor(squareContains) == BLACK) {
                addMove(board, moves, sq, nextSq, index);
            }
        } else {
            while (squareContains == EMPTY) {
                addMove(board, moves, sq, nextSq, index);

                nextSq = nextSq + offsets[i];
                squareContains = getPieceSq120(nextSq, board);
            }
            if (getColor(squareContains) == WHITE) {
                addMove(board, moves, sq, nextSq, index);
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

static void generatePseudoRookMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    char offsets[8] = {-10, -1, 10, 1};
    generateSlidingMoves(board, moves, sq, index, offsets);
}

static void generatePseudoBishopMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    char offsets[8] = {-11, -9, 9, 11};
    generateSlidingMoves(board, moves, sq, index, offsets);
}

static void generatePseudoQueenMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                     int *index) {
    generatePseudoRookMoves(board, moves, sq, index);
    generatePseudoBishopMoves(board, moves, sq, index);
}

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {
    char piece = getPieceSq120(move.startSquare, board);
    setPiece(EMPTY, SQ120F(move.startSquare), SQ120R(move.startSquare), board);
    setPiece(piece, SQ120F(move.endSquare), SQ120R(move.endSquare), board);
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {
    char piece = getPieceSq120(move.endSquare, board);
    setPiece(move.captured, SQ120F(move.endSquare), SQ120R(move.endSquare),
             board);
    setPiece(piece, SQ120F(move.startSquare), SQ120R(move.startSquare), board);
}

// generate all legal moves and insert them into the moves list
void generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;
    for (int file = FILE_A; file <= FILE_H; file++) {
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            int sq = FR2SQ120(file, rank);
            char piece = board->board[sq];

            if (piece >= wP && piece <= wK && board->turn == BLACK) {
                continue;
            } else if (piece >= bP && piece <= bK && board->turn == WHITE) {
                continue;
            }

            if (piece == EMPTY) {
                continue;
            } else if (piece == wK || piece == bK) {
                // printf("King on %c%d\n", file + 'a', rank + 1);
                generatePseudoKingMoves(board, moves, sq, &index);
            } else if (piece == wN || piece == bN) {
                // printf("Knight on %c%d\n", file + 'a', rank + 1);
                generatePseudoKnightMoves(board, moves, sq, &index);
            } else if (piece == wB || piece == bB) {
                // printf("Bishop on %c%d\n", file + 'a', rank + 1);
                generatePseudoBishopMoves(board, moves, sq, &index);
            } else if (piece == wR || piece == bR) {
                // printf("Rook on %c%d\n", file + 'a', rank + 1);
                generatePseudoRookMoves(board, moves, sq, &index);
            } else if (piece == wQ || piece == bQ) {
                // printf("Queen on %c%d\n", file + 'a', rank + 1);
                generatePseudoQueenMoves(board, moves, sq, &index);
            }
        }
    }

    printf("%d total moves\n", index);

    // for (int i = 0; i < index; i++) {
    //     printf("From %d to %d\n", moves[i].startSquare, moves[i].endSquare);
    // }
}
