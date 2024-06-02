#include "defs.h"
#include <assert.h>
#include <stdio.h>

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {
    // printf("move %d to %d\n",move.startSquare,move.endSquare);
    int piece = getPieceSq120(move.startSquare, board);
    setPiece(EMPTY, SQ120F(move.startSquare), SQ120R(move.startSquare), board);
    setPiece(piece, SQ120F(move.endSquare), SQ120R(move.endSquare), board);

    board->turn = notColor(board->turn);
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {
    // printf("unmove %d to %d\n",move.endSquare,move.startSquare);
    int piece = getPieceSq120(move.endSquare, board);
    setPiece(move.captured, SQ120F(move.endSquare), SQ120R(move.endSquare),
             board);
    setPiece(piece, SQ120F(move.startSquare), SQ120R(move.startSquare), board);
    board->turn = notColor(board->turn);
}

// add a move to the provided moves array
static void addMove(BOARD_STATE *board, MOVE *moves, int start, int end,
                    int captured, int enpassant, int twopawnmove,
                    int castled, int *index) {
    // printf("Move to sq %d\n", end);
    moves[*index].startSquare = start;
    moves[*index].endSquare = end;
    moves[*index].captured = captured;
    moves[*index].enpassant = enpassant;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castled = castled;
    (*index)++;
}

// generate all legal moves using array of offsets
// move from sq to sq+offset[i]. If square is empty or enemy piece, add to moves
// list. Used for kings and knights
static void generateSimpleMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                int *index, int *offsets) {
    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);

    int enemycolor = notColor(color);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);
        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        if (squareContains == EMPTY || getColor(squareContains) == enemycolor) {
            // printf("Move to sq %d\n", nextSq);
            addMove(board, moves, sq, nextSq, squareContains, OFFBOARD,
                    FALSE, NO_CASTLE, index);
        }
    }
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, int *offsets) {

    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);
    int enemycolor = notColor(color);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        while (squareContains == EMPTY) {
            addMove(board, moves, sq, nextSq, squareContains, OFFBOARD,
                    FALSE, NO_CASTLE, index);

            nextSq = nextSq + offsets[i];
            squareContains = getPieceSq120(nextSq, board);
        }
        if (getColor(squareContains) == enemycolor) {
            addMove(board, moves, sq, nextSq, squareContains, OFFBOARD,
                    FALSE, NO_CASTLE, index);
        }
    }
}

static void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);

    if (color == WHITE) {
        int one = sq + 1;
        int two = sq + 2;
        int left = sq - 9;
        int right = sq + 11;
        if (getPieceSq120(one, board) == EMPTY) {
            addMove(board, moves, sq, one, EMPTY, OFFBOARD, FALSE, NO_CASTLE, index);
        }

        int rPiece = getPieceSq120(right, board);
        int lPiece = getPieceSq120(left, board);

        if (getColor(lPiece) == BLACK) {
            addMove(board, moves, sq, left, lPiece, OFFBOARD, FALSE, NO_CASTLE, index);
        }

        if (getColor(rPiece) == BLACK) {
            addMove(board, moves, sq, right, rPiece, OFFBOARD, FALSE, NO_CASTLE,
                    index);
        }

        if (SQ120R(sq) == RANK_2 && getPieceSq120(one, board) == EMPTY &&
            getPieceSq120(two, board) == EMPTY) {
            addMove(board, moves, sq, two, EMPTY, OFFBOARD, TRUE, NO_CASTLE, index);
        }

    } else {
        int one = sq - 1;
        int two = sq - 2;
        int left = sq - 11;
        int right = sq + 9;
        if (getPieceSq120(one, board) == EMPTY) {
            addMove(board, moves, sq, one, EMPTY, OFFBOARD, FALSE, NO_CASTLE, index);
        }

        int rPiece = getPieceSq120(right, board);
        int lPiece = getPieceSq120(left, board);

        if (getColor(lPiece) == WHITE) {
            addMove(board, moves, sq, left, lPiece, OFFBOARD, FALSE, NO_CASTLE, index);
        }

        if (getColor(rPiece) == WHITE) {
            addMove(board, moves, sq, right, rPiece, OFFBOARD, FALSE, NO_CASTLE,
                    index);
        }

        if (SQ120R(sq) == RANK_7 && getPieceSq120(one, board) == EMPTY &&
            getPieceSq120(two, board) == EMPTY) {
            addMove(board, moves, sq, two, EMPTY, OFFBOARD, TRUE, NO_CASTLE, index);
        }
    }
}

static void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int offsets[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    generateSimpleMoves(board, moves, sq, index, offsets);
}

static void generatePseudoKnightMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    int offsets[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    generateSimpleMoves(board, moves, sq, index, offsets);
}

static void generatePseudoRookMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int offsets[8] = {-10, -1, 10, 1};
    generateSlidingMoves(board, moves, sq, index, offsets);
}

static void generatePseudoBishopMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    int offsets[8] = {-11, -9, 9, 11};
    generateSlidingMoves(board, moves, sq, index, offsets);
}

static void generatePseudoQueenMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                     int *index) {
    generatePseudoRookMoves(board, moves, sq, index);
    generatePseudoBishopMoves(board, moves, sq, index);
}

static int isAttackedSliding(BOARD_STATE *board, int sq, int *offsets, int wRB,
                             int bRB) {
    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);
    int enemycolor = notColor(color);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        while (squareContains == EMPTY) {
            nextSq = nextSq + offsets[i];
            squareContains = getPieceSq120(nextSq, board);
        }
        if (getColor(squareContains) == enemycolor) {
            if (squareContains == bQ || squareContains == wQ ||
                squareContains == wRB || squareContains == bRB) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static int isAttackedSimple(BOARD_STATE *board, int sq, int *offsets) {

    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);

    for (int i = 0; i < (int)sizeof(offsets); i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        if (color == WHITE) {
            if (getColor(squareContains) == BLACK) {
                return TRUE;
            }
        } else {
            if (getColor(squareContains) == WHITE) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

int isAttacked(BOARD_STATE *board, int sq) {
    int piece = getPieceSq120(sq, board);
    int color = getColor(piece);

    int pawn[2] = {-9, 11};

    if (color == BLACK) {
        pawn[0] = 9;
        pawn[1] = -11;
    }

    int king[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    int knight[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    int rook[4] = {-10, -1, 10, 1};
    int bishop[4] = {-11, -9, 9, 11};

    return isAttackedSimple(board, sq, king) |
           isAttackedSimple(board, sq, knight) |
           isAttackedSimple(board, sq, pawn) |
           isAttackedSliding(board, sq, rook, wR, bR) |
           isAttackedSliding(board, sq, bishop, wB, bB);
}

int isLegalMove(BOARD_STATE *board, MOVE move) {
    makeMove(board, move);
    int color = board->turn;
    int kingsq = board->kings[color];
    int check = isAttacked(board, kingsq);
    unmakeMove(board, move);
    return !check;
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;
    for (int file = FILE_A; file <= FILE_H; file++) {
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            int sq = FR2SQ120(file, rank);
            int piece = board->board[sq];

            // only calc white/black moves on white/black's turn
            if (piece >= wP && piece <= wK && board->turn == BLACK) {
                continue;
            } else if (piece >= bP && piece <= bK && board->turn == WHITE) {
                continue;
            }

            if (piece == EMPTY) {
                continue;
            } else if (piece == wK || piece == bK) {
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
            } else if (piece == wP || piece == bP) {
                // printf("Queen on %c%d\n", file + 'a', rank + 1);
                generatePseudoPawnMoves(board, moves, sq, &index);
            }
        }
    }

    return index;

    // for (int i = 0; i < index; i++) {
    //     printf("From %d to %d\n", moves[i].startSquare, moves[i].endSquare);
    // }
}
