#include "defs.h"
#include <assert.h>
#include <stdio.h>

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(ULL bb) {

    const int index64[64] = {0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28,
                             16, 3,  61, 54, 58, 35, 52, 50, 42, 21, 44, 38, 32,
                             29, 23, 17, 11, 4,  62, 46, 55, 26, 59, 40, 36, 15,
                             53, 34, 51, 20, 43, 31, 22, 10, 45, 25, 39, 14, 33,
                             19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63};

    const ULL debruijn64 = 0x03f79d71b4cb0a89;
    return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}

/**
 * bitScanReverse
 * @authors Kim Walisch, Mark Dickinson
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of most significant one bit
 */
int bitScanReverse(ULL bb) {

    const int index64[64] = {0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28,
                             16, 3,  61, 54, 58, 35, 52, 50, 42, 21, 44, 38, 32,
                             29, 23, 17, 11, 4,  62, 46, 55, 26, 59, 40, 36, 15,
                             53, 34, 51, 20, 43, 31, 22, 10, 45, 25, 39, 14, 33,
                             19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63};
    const ULL debruijn64 = 0x03f79d71b4cb0a89;
    // assert(bb != 0);
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return index64[(bb * debruijn64) >> 58];
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

int countBits(ULL b) {
    int r;
    for (r = 0; b; ++r, b &= b - 1)
        ;
    return r;
}
