#include "defs.h"
#include <assert.h>
#include <stdio.h>

int isEmptySquare(int sq, BOARD_STATE *board) {
    ULL bb = (board->bitboard[bbWhite] | board->bitboard[bbBlack]);
    ULL val = CHECKBIT(bb, SQ120SQ64(sq));

    return (ONBOARD(sq) && !val);
}

// TODO: fix
int hasEmptyEnemyPiece120(int sq, int color, BOARD_STATE *board) {
    ULL set = CHECKBIT(board->bitboard[color], SQ120SQ64(sq));
    return (ONBOARD(sq) && (!set));
}

// TODO: fix
int hasEnemyPiece120(int sq, BOARD_STATE *board) {
    ULL set = CHECKBIT(board->bitboard[NOTCOLOR(board->turn)], SQ120SQ64(sq));
    return (ONBOARD(sq) && set);
}

int getGenericPieceSq120(int sq, BOARD_STATE *board) {
    int sq64 = SQ120SQ64(sq);
    for (int i = bbPawn; i < bbAny; ++i) {
        if (CHECKBIT(board->bitboard[i], sq64)) {
            return i;
        };
    }
    return -1;
}

int getColorSq120(int sq, BOARD_STATE *board) {
    ULL any = CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                       SQ120SQ64(sq));
    if (!any) {
        return BOTH;
    }
    ULL white = CHECKBIT(board->bitboard[bbWhite], SQ120SQ64(sq));
    if (white) {
        return WHITE;
    }
    return BLACK;
}

// return piece on square given by 120 index
int getPieceSq120(int sq, BOARD_STATE *board) {

    if (!ONBOARD(sq)) {
        return OFFBOARD;
    }

    int sq64 = SQ120SQ64(sq);

    ULL set =
        CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack], sq64);

    if (!set) {
        return EMPTY;
    }

    for (int i = bbPawn; i < bbAny; ++i) {

        ULL val = CHECKBIT(board->bitboard[i], sq64);
        if (val) {
            if (CHECKBIT((board->bitboard[bbWhite]), sq64)) {
                return TOWHITE(i);
            }
            return TOBLACK(i);
        }
    }
}

// return piece on given square by file/rank
int getPieceFR(int file, int rank, BOARD_STATE *board) {
    int sq = FR2SQ120(file, rank);
    return getPieceSq120(sq, board);
}

void setPiece120(int piece, int sq, BOARD_STATE *board) {

    const int color = COLOR(piece);
    const int generic = GENERIC(piece);

    int sq64 = SQ120SQ64(sq);

    for (int i = 0; i < bbLength; ++i) {
        CLEARBIT(board->bitboard[i], sq64);
    }

    // add piece to bitboard
    if (piece != EMPTY) {
        SETBIT(board->bitboard[generic], sq64);
        SETBIT(board->bitboard[color], sq64);
        SETBIT(board->bitboard[bbAny], sq64);
    }

    // update king placement
    if (piece == wK) {
        board->kings[WHITE] = sq;
    } else if (piece == bK) {
        board->kings[BLACK] = sq;
    }
}

// place piece on given square by updating board/bitboards
void setPiece(int piece, int file, int rank, BOARD_STATE *board) {

    int sq = FR2SQ120(file, rank);
    setPiece120(piece, sq, board);
}
