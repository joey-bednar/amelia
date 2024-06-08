#include "defs.h"
#include <assert.h>
#include <stdio.h>

void printBits(unsigned long long num) {

    printf("\n");
    for (int i = 0; i < 64; ++i) {
        if (num >> i & 1ULL)
            putchar('1');
        else
            putchar('0');
    }
    printf("\n");
}

void printBitboard(unsigned long long bitboard) {

    printf("\n");
    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            printf("\n");
        }
        if (bitboard >> i & 1ULL)
            printf("1 ");
        else
            printf("0 ");
    }
    printf("\n");
}

void printBitboardIndex64() {

    printf("\n");
    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            printf("\n");
        }
        if (i < 10) {
            printf(" %d ", i);
        } else {
            printf("%d ", i);
        }
    }
    printf("\n");
}

void printBitboardIndex120() {

    printf("\n");
    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("%d ", SQ64SQ120(i));
    }
    printf("\n");
}

int countBits(unsigned long long b) {
    int r;
    for (r = 0; b; r++, b &= b - 1)
        ;
    return r;
}

// TODO: add FEN->board, board->FEN
