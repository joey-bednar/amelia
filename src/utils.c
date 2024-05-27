#include "defs.h"
#include <assert.h>
#include <stdio.h>

void printBits(unsigned long long num) {
    int bits = sizeof(num) * 8;
    unsigned long mask = 1UL << (bits - 1);

    for (int i = 0; i < bits; i++) {
        putchar((num & mask) ? '1' : '0');
        num <<= 1;
    }
}

void printBitboard(unsigned long long bitboard) {
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            char sq = FR2SQ64(file, rank);

            char val = (checkBitboard(file, rank, &bitboard));
            printf("%d ", val);
        }
        printf("\n");
    }
}

int countBits(unsigned long long b) {
    int r;
    for (r = 0; b; r++, b &= b - 1)
        ;
    return r;
}

// return color of piece
char getColor(char piece) {
    if (piece >= wP && piece <= wK) {
        return WHITE;
    } else if (piece >= bP && piece <= bK) {
        return BLACK;
    }
    return BOTH;
}

// TODO: add FEN->board, board->FEN
