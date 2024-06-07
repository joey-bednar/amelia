#include "defs.h"
#include <assert.h>
#include <stdio.h>

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {

    // TODO: cleanup
    int piece = getPieceSq120(move.startSquare, board);
    setPiece120(EMPTY, move.startSquare, board);
    setPiece120(piece, move.endSquare, board);

    int offset[2] = {S, N};

    if (move.twopawnmove) {
        board->enpassant = move.endSquare + offset[board->turn];
        if (!epMap[board->enpassant]) {
            printf("error %d", board->enpassant);
            assert(FALSE);
        }
    } else {
        board->enpassant = OFFBOARD;
    }

    if (move.epcapture) {
        board->enpassant = OFFBOARD;
        setPiece120(EMPTY, move.endSquare + offset[board->turn], board);
    } else if (move.promotion != EMPTY) {
        setPiece120(move.promotion, move.endSquare, board);
    }
    board->turn = !(board->turn);
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {

    // TODO: cleanup
    int offset = S;
    if (board->turn == WHITE) {
        offset = N;
    }

    // printf("unmove %d to %d\n",move.endSquare,move.startSquare);
    int piece = getPieceSq120(move.endSquare, board);

    if (move.epcapture) {
        board->enpassant = move.endSquare;
        setPiece120(move.captured, move.endSquare + offset, board);
        setPiece120(EMPTY, move.endSquare, board);
        setPiece120(piece, move.startSquare, board);
        board->turn = !(board->turn);
        return;
    }

    setPiece120(move.captured, move.endSquare, board);
    if (move.promotion == EMPTY) {
        setPiece120(piece, move.startSquare, board);
    } else {
        int pawn = wP;
        if (COLOR(piece) == BLACK) {
            pawn = bP;
        }
        setPiece120(pawn, move.startSquare, board);
    }

    if (move.twopawnmove) {
        board->enpassant = move.priorep;
    }

    board->turn = !(board->turn);
}

// add a move to the provided moves array
static void addMove(BOARD_STATE *board, MOVE *moves, int start, int end,
                    int captured, int epcapture, int twopawnmove, int castled,
                    int promotion, int *index) {
    // printf("Move to sq %d\n", end);
    moves[*index].startSquare = start;
    moves[*index].endSquare = end;
    moves[*index].captured = captured;
    moves[*index].epcapture = epcapture;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castled = castled;
    moves[*index].promotion = promotion;
    moves[*index].priorep = board->enpassant;
    (*index)++;
}

// generate all legal moves using array of offsets
// move from sq to sq+offset[i]. If square is empty or enemy piece, add to moves
// list. Used for kings and knights
static void generateSimpleMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                int *index, int *offsets, int offsetssize) {
    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < offsetssize; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);
        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        if (squareContains == EMPTY ||
            COLOR(squareContains) == NOTCOLOR(piece)) {
            // printf("Move to sq %d\n", nextSq);
            addMove(board, moves, sq, nextSq, squareContains, FALSE, FALSE,
                    NO_CASTLE, EMPTY, index);
        }
    }
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, int *offsets, int offsetssize) {

    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < offsetssize; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        while (squareContains == EMPTY) {
            addMove(board, moves, sq, nextSq, squareContains, FALSE, FALSE,
                    NO_CASTLE, EMPTY, index);

            nextSq = nextSq + offsets[i];
            squareContains = getPieceSq120(nextSq, board);
        }
        if (COLOR(squareContains) == NOTCOLOR(piece)) {
            addMove(board, moves, sq, nextSq, squareContains, FALSE, FALSE,
                    NO_CASTLE, EMPTY, index);
        }
    }
}

static void addPromotions(BOARD_STATE *board, MOVE *moves, int start, int end,
                          int captured, int color, int *index) {
    int promoteTo[4] = {wQ, wN, wR, wB};
    if (color == BLACK) {
        promoteTo[0] = bQ;
        promoteTo[1] = bN;
        promoteTo[2] = bR;
        promoteTo[3] = bB;
    }
    for (int i = 0; i < 4; i++) {
        addMove(board, moves, start, end, captured, FALSE, FALSE, FALSE,
                promoteTo[i], index);
    }
}

static void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int piece = getPieceSq120(sq, board);
    int color = COLOR(piece);

    int secondrank = RANK_2;
    int eighthrank = RANK_8;
    int offset[4] = {1, 2, -9, 11};
    int enemypawn = bP;
    if (color == BLACK) {
        offset[0] = -1;
        offset[1] = -2;
        offset[2] = 9;
        offset[3] = -11;
        secondrank = RANK_7;
        eighthrank = RANK_1;
        enemypawn = wP;
    }

    int one = sq + offset[0];
    int two = sq + offset[1];

    static int enpassant = 0;

    // up one
    if (getPieceSq120(one, board) == EMPTY) {
        if (SQ120R(one) == eighthrank) {
            addPromotions(board, moves, sq, one, EMPTY, color, index);
        } else {
            addMove(board, moves, sq, one, EMPTY, FALSE, FALSE, NO_CASTLE,
                    EMPTY, index);
        }
    }

    // captures and en passant
    for (int i = 2; i <= 3; i++) {
        int enemysquare = sq + offset[i];
        int enemypiece = getPieceSq120(enemysquare, board);
        if (COLOR(enemypiece) == NOTCOLOR(piece)) {

            if (SQ120R(enemysquare) == eighthrank) {
                addPromotions(board, moves, sq, enemysquare, enemypiece, color,
                              index);
            } else {
                addMove(board, moves, sq, enemysquare, enemypiece, FALSE, FALSE,
                        NO_CASTLE, EMPTY, index);
            }
        } else if (epMap[enemysquare] && enemysquare == board->enpassant) {
            enpassant++;
            // TODO: test further

            // printf("en passant %d\n", enpassant);
            addMove(board, moves, sq, enemysquare, enemypawn, TRUE, FALSE,
                    NO_CASTLE, EMPTY, index);
        }
    }

    // up two
    if (SQ120R(sq) == secondrank && getPieceSq120(one, board) == EMPTY &&
        getPieceSq120(two, board) == EMPTY) {
        addMove(board, moves, sq, two, EMPTY, FALSE, TRUE, NO_CASTLE, EMPTY,
                index);
    }
}

static void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int offsets[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    generateSimpleMoves(board, moves, sq, index, offsets, 8);
}

static void generatePseudoKnightMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    int offsets[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    generateSimpleMoves(board, moves, sq, index, offsets, 8);
}

static void generatePseudoRookMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int offsets[4] = {-10, -1, 10, 1};
    generateSlidingMoves(board, moves, sq, index, offsets, 4);
}

static void generatePseudoBishopMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    int offsets[4] = {-11, -9, 9, 11};
    generateSlidingMoves(board, moves, sq, index, offsets, 4);
}

static void generatePseudoQueenMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                     int *index) {
    generatePseudoRookMoves(board, moves, sq, index);
    generatePseudoBishopMoves(board, moves, sq, index);
}

static int isAttackedSliding(BOARD_STATE *board, int sq, int *offsets,
                             int sizeoffset, int enemycolor, int wRB, int bRB) {
    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        while (squareContains == EMPTY) {
            nextSq = nextSq + offsets[i];
            squareContains = getPieceSq120(nextSq, board);
        }
        if (COLOR(squareContains) == enemycolor) {

            if (squareContains == bQ || squareContains == wQ ||
                squareContains == wRB || squareContains == bRB) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static int isAttackedSimple(BOARD_STATE *board, int sq, int *offsets,
                            int sizeoffset, int enemycolor, int wKNP,
                            int bKNP) {

    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        if (COLOR(squareContains) == enemycolor) {
            if (squareContains == wKNP || squareContains == bKNP) {

                return TRUE;
            }
        }
    }

    return FALSE;
}

int isAttacked(BOARD_STATE *board, int sq, int enemycolor) {
    assert(enemycolor != BOTH);

    int pawn[2] = {-9, 11};
    if (enemycolor == WHITE) {
        pawn[0] = 9;
        pawn[1] = -11;
    }

    int king[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    int knight[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    int rook[4] = {-10, -1, 10, 1};
    int bishop[4] = {-11, -9, 9, 11};

    return isAttackedSimple(board, sq, king, 8, enemycolor, wK, bK) ||
           isAttackedSimple(board, sq, knight, 8, enemycolor, wN, bN) ||
           isAttackedSimple(board, sq, pawn, 2, enemycolor, wP, bP) ||
           isAttackedSliding(board, sq, rook, 4, enemycolor, wR, bR) ||
           isAttackedSliding(board, sq, bishop, 4, enemycolor, wB, bB);
}

static int inCheck(BOARD_STATE *board, int color) {
    return isAttacked(board, board->kings[color], !(board->kings[color]));
}

int isLegalMove(BOARD_STATE *board, MOVE move) {
    int color = board->turn;
    makeMove(board, move);
    int kingsq = board->kings[color];
    int check = isAttacked(board, kingsq, board->turn);
    unmakeMove(board, move);
    return !check;
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;

    ULL bb;

    // only calc white/black moves on white/black's turn
    switch (board->turn) {
    case WHITE:
        bb = board->pieces[wK] | board->pieces[wQ] | board->pieces[wR] |
             board->pieces[wB] | board->pieces[wN] | board->pieces[wP];
        break;

    case BLACK:
        bb = board->pieces[bK] | board->pieces[bQ] | board->pieces[bR] |
             board->pieces[bB] | board->pieces[bN] | board->pieces[bP];
        break;
    default:
        assert(FALSE);
    }

    while (bb != 0) {
        int index64 = bitScanForward(bb);
        int sq = SQ64SQ120(index64);

        int piece = getPieceSq120(sq, board);

        ULL mask = (~1ULL << (index64));
        bb &= mask;

        if (piece == wP || piece == bP) {
            generatePseudoPawnMoves(board, moves, sq, &index);
        } else if (piece == wR || piece == bR) {
            generatePseudoRookMoves(board, moves, sq, &index);
        } else if (piece == wB || piece == bB) {
            generatePseudoBishopMoves(board, moves, sq, &index);
        } else if (piece == wN || piece == bN) {
            generatePseudoKnightMoves(board, moves, sq, &index);
        } else if (piece == wK || piece == bK) {
            generatePseudoKingMoves(board, moves, sq, &index);
        } else if (piece == wQ || piece == bQ) {
            generatePseudoQueenMoves(board, moves, sq, &index);
        }
    }

    return index;
}

// // generate all legal moves and insert them into the moves list
// int generateMoves(BOARD_STATE *board, MOVE *moves) {
//
//     int index = 0;
//
//     ULL test = board->pieces[EMPTY];
//
//     for (int i = 0; i < 64; i++) {
//
//         int found = (test & 1UL);
//         test >>= 1;
//
//         if (!found) {
//             continue;
//         }
//
//         int sq = SQ64SQ120(i);
//         int piece = getPieceSq120(sq, board);
//
//         // only calc white/black moves on white/black's turn
//         if (COLOR(piece) != board->turn) {
//             continue;
//         } else if (piece == wK || piece == bK) {
//             generatePseudoKingMoves(board, moves, sq, &index);
//         } else if (piece == wN || piece == bN) {
//             // printf("Knight on %c%d\n", file + 'a', rank + 1);
//             generatePseudoKnightMoves(board, moves, sq, &index);
//         } else if (piece == wB || piece == bB) {
//             // printf("Bishop on %c%d\n", file + 'a', rank + 1);
//             generatePseudoBishopMoves(board, moves, sq, &index);
//         } else if (piece == wR || piece == bR) {
//             // printf("Rook on %c%d\n", file + 'a', rank + 1);
//             generatePseudoRookMoves(board, moves, sq, &index);
//         } else if (piece == wQ || piece == bQ) {
//             // printf("Queen on %c%d\n", file + 'a', rank + 1);
//             generatePseudoQueenMoves(board, moves, sq, &index);
//         } else if (piece == wP || piece == bP) {
//             // printf("Queen on %c%d\n", file + 'a', rank + 1);
//             generatePseudoPawnMoves(board, moves, sq, &index);
//         }
//     }
//     return index;
// }
