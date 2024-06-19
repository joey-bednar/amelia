#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static ULL get64rand() {
    return (((ULL)rand() << 0) & 0x000000000000FFFFull) |
           (((ULL)rand() << 16) & 0x00000000FFFF0000ull) |
           (((ULL)rand() << 32) & 0x0000FFFF00000000ull) |
           (((ULL)rand() << 48) & 0xFFFF000000000000ull);
}

// init random zobrist values
void initZobrist() {
    for (int piece = wP; piece <= bK; piece++) {
        for (int sq = 0; sq < 64; sq++) {
            zobrist_vals[piece - 1][sq] = get64rand();
        }
    }
    ULL zobristB2M = get64rand();
}

// constructs hash of position
void loadZobrist(BOARD_STATE *board) {
    if (board->turn == BLACK) {
        board->hash ^= zobristB2M;
    }
    ULL bb = board->bitboard[WHITE] | board->bitboard[BLACK];
    BITLOOP(bb) {
        int sq64 = bitScanForward(bb);
        int piece = getPieceSq120(SQ64SQ120(sq64), board);
        board->hash ^= zobrist_vals[piece - 1][sq64];
    }
}

void updateZobrist(int sq64, int piece, BOARD_STATE *board) {
    board->hash ^= zobrist_vals[piece - 1][sq64];
}

void turnZobrist(BOARD_STATE *board) {
    board->hash ^= zobristB2M;
}
