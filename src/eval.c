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
            if ((board->hash ^ zobristB2M) == board->playedmoves[i]) {
                ++count;
            }
        }

        if (count >= 3) {
            return TRUE;
        }
    }

    return FALSE;
}

// returns eval for material count
static int computeMaterialTotals(ULL bb, int color, BOARD_STATE *board) {
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
    const int arrCenterManhattanDistance[64] = {
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

    int CMD =
        arrCenterManhattanDistance[SQ120SQ64(board->kings[!winningcolor])];
    int MD = manhattanDistance(SQ120SQ64(board->kings[WHITE]),
                               SQ120SQ64(board->kings[BLACK]));

    int mopUp = (int)(4.7 * CMD + 1.6 * (14 - MD));

    // int myCMD =
    // (int)(6-arrCenterManhattanDistance[SQ120SQ64(board->kings[board->turn])]);
    // int yourCMD =
    // (int)(6-arrCenterManhattanDistance[SQ120SQ64(board->kings[!board->turn])]);
    // int newmopUp = myCMD - yourCMD;
    //
    // printf("cmd: %d, md: %d, mop: %d\n", CMD, MD, mopUp);
    //
    // printf("%d %d\n",SQ120SQ64(E4),SQ120SQ64(E1));
    // printf("%d
    // %d\n",arrCenterManhattanDistance[SQ120SQ64(E4)],arrCenterManhattanDistance[SQ120SQ64(E1)]);

    return mopUp;
}

static int isEndgame(int totalMaterial) {
    if (totalMaterial < 2000) {
        return TRUE;
    }
    return FALSE;
}

// returns centipawn evaluation of current position
int eval(BOARD_STATE *board) {
    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    // compute material eval
    int myMaterial = computeMaterialTotals(mine, board->turn, board);
    int yourMaterial = computeMaterialTotals(yours, !board->turn, board);
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
