#include "defs.h"
#include <assert.h>

// return TRUE if sq(120) is attacked by an enemy piece. calculated
// using preset attack bitboards
static int isAttackedPreset(BOARD_STATE *board, int sq, int enemycolor,
                            ULL bitboard, int genericPiece) {

    // check if enemy piece exists on possible attacking squares
    ULL bb = (bitboard & board->bitboard[genericPiece] &
              board->bitboard[enemycolor]);

    return !(ONBOARD(sq) && bb == 0);
}

static int isAttackedRay(BOARD_STATE *board, int sq120, int enemycolor) {
    if (!ONBOARD(sq120)) {
        return FALSE;
    }

    const int piece[8] = {bbRook, bbBishop, bbRook, bbBishop,
                          bbRook, bbBishop, bbRook, bbBishop};

    int sq64 = SQ120SQ64(sq120);

    ULL allpieces = board->bitboard[WHITE] | board->bitboard[BLACK];

    for (int dir = 0; dir < 8; dir++) {

        ULL enemypieces =
            board->bitboard[enemycolor] &
            (board->bitboard[piece[dir]] | board->bitboard[bbQueen]);

        ULL attacked = slidingRay[dir][sq64] & enemypieces;

        if (!attacked) {
            continue;
        }

        int nearSq;
        if (dir < 4) {
            nearSq = bitScanForward(slidingRay[dir][sq64] & allpieces);
        } else {
            nearSq = bitScanReverse(slidingRay[dir][sq64] & allpieces);
        }

        if (CHECKBIT(enemypieces, nearSq)) {
            return TRUE;
        }
    }

    return FALSE;
}

// return TRUE if sq(120) is attacked by an enemy pawn
static int isAttackedPawn(BOARD_STATE *board, int sq, int enemycolor) {

    ULL pawns = board->bitboard[bbPawn] & board->bitboard[enemycolor];

    ULL right = 0ULL;
    ULL left = 0ULL;

    switch (enemycolor) {
    case WHITE:
        // white pawns
        right = (pawns << 9);
        left = (pawns >> 7);
        break;

    case BLACK:
        // black pawns
        right = (pawns << 7);
        left = (pawns >> 9);
        break;
    }

    ULL bbp = (right & (~0ULL << 8)) | (left & (~0ULL >> 8));

    return CHECKBIT(bbp, SQ120SQ64(sq));
}

// return TRUE if sq(120) is attacked by the enemy color
int isAttacked(BOARD_STATE *board, int sq, int enemycolor) {

    return isAttackedPreset(board, sq, enemycolor, KINGBB(SQ120SQ64(sq)),
                            bbKing) ||
           isAttackedPreset(board, sq, enemycolor, KNIGHTBB(SQ120SQ64(sq)),
                            bbKnight) ||
           isAttackedPawn(board, sq, enemycolor) ||

           isAttackedRay(board, sq, enemycolor);
}

// return TRUE if move is legal
int isLegalMove(BOARD_STATE *board, MOVE move) {
    int color = board->turn;

    makeMove(board, move);
    int kingsq = SQ64SQ120(getKingSq(board, color));
    int check = isAttacked(board, kingsq, board->turn);
    unmakeMove(board, move);
    return !check;
}

// return TRUE if move is a check
int isCheck(BOARD_STATE *board, MOVE move) {
    makeMove(board, move);
    int kingsq = SQ64SQ120(getKingSq(board, board->turn));
    int check = isAttacked(board, kingsq, !board->turn);
    unmakeMove(board, move);
    return check;
}
