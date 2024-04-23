#include "defs.h"
#include <assert.h>
#include <stdio.h>

// clears the bitboard of any pieces
void clearBitboard(unsigned long long *bitboard) {
    *bitboard = 0;
}

// adds piece to bitboard on the given square
void addBitboard(int file, int rank, unsigned long long *bitboard) {
    int coord = FR2SQ64(file, rank);
    *bitboard |= (0x01UL << coord);
}

// removes piece from bitboard on the given square
void removeBitboard(int file, int rank, unsigned long long *bitboard) {
    int coord = FR2SQ64(file, rank);
    *bitboard &= ~(0x01UL << coord);
}

// return true if piece is on square
char checkBitboard(int file, int rank, unsigned long long *bitboard) {
    int coord = FR2SQ64(file, rank);
    return (char)(*bitboard & (0x01UL << coord));
}
