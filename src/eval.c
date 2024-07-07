#include "defs.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// returns true if position has been repeated three times
int isThreeFold(BOARD_STATE *board) {
    if (board->halfmove >= 4) {
        int end = 2 * (board->fullmove - 1) + board->turn;
        int start = 0; // end - board->halfmove;
        int count = 0;
        for (int i = start; i <= end; ++i) {
            if ((board->hash ^ zobristB2M) == board->playedmoves[i].hash) {
                ++count;
            }
        }

        if (count >= 3) {
            return TRUE;
        }
    }

    return FALSE;
}

int isInsufficientMaterial(BOARD_STATE *board) {

    // any queen/rook/pawn can mate
    if (board->bitboard[bbQueen] > 0ull || board->bitboard[bbRook] > 0ull ||
        board->bitboard[bbPawn] > 0ull) {
        return FALSE;
    }

    ULL whiteBishop = board->bitboard[bbBishop] & board->bitboard[WHITE];
    ULL blackBishop = board->bitboard[bbBishop] & board->bitboard[BLACK];

    ULL whiteKnight = board->bitboard[bbKnight] & board->bitboard[WHITE];
    ULL blackKnight = board->bitboard[bbKnight] & board->bitboard[BLACK];

    // bishop/knight can mate
    if (whiteBishop > 0ull && whiteKnight > 0ull) {
        return FALSE;
    }
    if (blackBishop > 0ull && blackKnight > 0ull) {
        return FALSE;
    }

    // bishop pair can mate
    int whiteBishopCount = countBits(whiteBishop);
    int blackBishopCount = countBits(blackBishop);
    if (whiteBishopCount > 1 || blackBishopCount > 1) {
        return FALSE;
    }

    return TRUE;
}

// returns eval for material count
static int computeMaterialTotals(ULL bb, BOARD_STATE *board) {
    int total = 0;

    const int val[] = {100, 320, 330, 500, 900, 0};

    BITLOOP(bb) {
        int index64 = bitScanForward(bb);

        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        total += val[piece - bbPawn];
    }

    return total;
}

// returns eval for piece square tables
static int computePieceSqTotals(ULL bb, int color, int endgame,
                                BOARD_STATE *board) {
    int total = 0;

    if (endgame) {
        bb &= ~board->bitboard[bbKing];
    }

    BITLOOP(bb) {
        int index64 = bitScanForward(bb);
        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        if (color == BLACK) {
            index64 ^= 7;
        }

        switch (piece) {
        case bbPawn:
            total += pawnSqTable[index64];
            break;
        case bbRook:
            total += rookSqTable[index64];
            break;
        case bbBishop:
            total += bishopSqTable[index64];
            break;
        case bbKnight:
            total += knightSqTable[index64];
            break;
        case bbQueen:
            total += queenSqTable[index64];
            break;
        case bbKing:
            total += kingSqTable[index64];
            break;
        }
    }

    return total;
}

static int manhattanDistance(int sq1, int sq2) {
    int file1, file2, rank1, rank2;
    int rankDistance, fileDistance;
    file1 = sq1 & 7;
    file2 = sq2 & 7;
    rank1 = sq1 >> 3;
    rank2 = sq2 >> 3;
    rankDistance = abs(rank2 - rank1);
    fileDistance = abs(file2 - file1);
    return rankDistance + fileDistance;
}

static int computeMopUp(int winningcolor, BOARD_STATE *board) {
    // clang-format off
    const int centerManhattanDistance[64] = {
      6, 5, 4, 3, 3, 4, 5, 6,
      5, 4, 3, 2, 2, 3, 4, 5,
      4, 3, 2, 1, 1, 2, 3, 4,
      3, 2, 1, 0, 0, 1, 2, 3,
      3, 2, 1, 0, 0, 1, 2, 3,
      4, 3, 2, 1, 1, 2, 3, 4,
      5, 4, 3, 2, 2, 3, 4, 5,
      6, 5, 4, 3, 3, 4, 5, 6
    };
    // clang-format on

    int CMD = centerManhattanDistance[getKingSq(board, !winningcolor)];
    int MD =
        manhattanDistance(getKingSq(board, WHITE), getKingSq(board, BLACK));

    int mopUp = (int)(4.7 * CMD + 1.6 * (14 - MD));

    return mopUp;
}

static int isEndgame(int totalMaterial) {
    return (totalMaterial < 2000);
}

// returns centipawn evaluation of current position
int eval(BOARD_STATE *board) {
    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    // compute material eval
    int myMaterial = computeMaterialTotals(mine, board);
    int yourMaterial = computeMaterialTotals(yours, board);
    total += myMaterial - yourMaterial;

    // compute mop up eval for endgames
    int endgame = isEndgame(myMaterial + yourMaterial);
    if (endgame) {
        int winningcolor;
        if (total > 0) {
            total += computeMopUp(board->turn, board);
        } else {
            total -= computeMopUp(!board->turn, board);
        }
    }

    // compute piece square eval
    total += computePieceSqTotals(mine, board->turn, endgame, board);
    total -= computePieceSqTotals(yours, !board->turn, endgame, board);

    return total;
}
