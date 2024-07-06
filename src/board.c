#include "defs.h"
#include <assert.h>
#include <stdio.h>

void clearPiece(BOARD_STATE *board, int piece, int sq) {
    // move piece to target square
    CLEARBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    updateZobrist(SQ120SQ64(sq), piece, board);
}

void placePiece(BOARD_STATE *board, int piece, int sq) {

    SETBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    updateZobrist(SQ120SQ64(sq), piece, board);
}

void updateCastling(BOARD_STATE *board, MOVE move) {

    int piece = getGenericPieceSq120(START120(move.compact), board);

    if (piece == bbRook) {
        if (START120(move.compact) == H1) {
            CLEARBIT(board->castle, WK_CASTLE);
        } else if (START120(move.compact) == A1) {
            CLEARBIT(board->castle, WQ_CASTLE);
        } else if (START120(move.compact) == H8) {
            CLEARBIT(board->castle, BK_CASTLE);
        } else if (START120(move.compact) == A8) {
            CLEARBIT(board->castle, BQ_CASTLE);
        }
    } else if (piece == bbKing) {
        if (START120(move.compact) == E1) {
            CLEARBIT(board->castle, WK_CASTLE);
            CLEARBIT(board->castle, WQ_CASTLE);
        } else if (END120(move.compact) == E8) {
            CLEARBIT(board->castle, BK_CASTLE);
            CLEARBIT(board->castle, BQ_CASTLE);
        }
    }
}

int isEmptySquare(int sq, BOARD_STATE *board) {
    ULL bb = (board->bitboard[bbWhite] | board->bitboard[bbBlack]);
    ULL val = CHECKBIT(bb, SQ120SQ64(sq));

    return (ONBOARD(sq) && !val);
}

int getGenericPieceSq120(int sq, BOARD_STATE *board) {
    int sq64 = SQ120SQ64(sq);
    for (int i = bbPawn; i <= bbKing; ++i) {
        if (CHECKBIT(board->bitboard[i], sq64)) {
            return i;
        };
    }
    return -1;
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

    for (int i = bbPawn; i <= bbKing; ++i) {

        ULL val = CHECKBIT(board->bitboard[i], sq64);
        if (val) {
            if (CHECKBIT((board->bitboard[bbWhite]), sq64)) {
                return TOWHITE(i);
            }
            return TOBLACK(i);
        }
    }

    // shouldn't get here
    return OFFBOARD;
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
        // SETBIT(board->bitboard[bbAny], sq64);
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
