#include "defs.h"
#include <assert.h>
#include <stdio.h>

int bitScanForward(ULL bb) {
    return __builtin_ffsll(bb) - 1;
}

int bitScanReverse(ULL bb) {
    return 63 - __builtin_clzll(bb);
}

// prints the pieces on the board
void printBoard(BOARD_STATE *board) {
    // int to represent a piece in FEN notation
    const int piece2char[] = {'.', 'P', 'N', 'B', 'R', 'Q', 'K',
                              'p', 'n', 'b', 'r', 'q', 'k'};
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        for (int file = FILE_A; file <= FILE_H; ++file) {
            int sq = FR2SQ120(file, rank);
            int display = piece2char[getPieceSq120(sq, board)];
            printf("%c ", display);
        }
        printf("\n");
    }
    if (board->turn == WHITE) {
        printf("White to move.\n");
    } else {
        printf("Black to move.\n");
    }
    printf("En passant: %d\n", board->enpassant);
    printf("Castling: %d\n", board->castle);
    printf("Half: %d, Full: %d\n", board->halfmove, board->fullmove);
    printf("Hash: %llu\n", board->hash);
}

// prints the pieces on the board
void printBoardIndex() {
    printf("\n");
    // int to represent a piece in FEN notation
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        for (int file = FILE_A; file <= FILE_H; ++file) {
            int sq = FR2SQ120(file, rank);
            printf("%d ", sq);
        }
        printf("\n");
    }
    printf("\n");
}

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

void printBitboard(ULL bitboard) {

    // ULL bbcopy = bitboard;

    // create a flipped copy
    ULL bbcopy = 0ull;
    for (int i = 0; i < 64; ++i) {
        int val = CHECKBIT(bitboard, i);
        if (val) {
            SETBIT(bbcopy, ((((i >> 3) | (i << 3)) & 63) ^ 56));
        }
    }

    // print flipped copy
    printf("\n");
    for (int i = 0; i < 64; ++i) {
        if (i % 8 == 0) {
            printf("\n");
        }
        if (bbcopy >> i & 1)
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

int countBits(ULL b) {
    return __builtin_popcountll(b);
}
