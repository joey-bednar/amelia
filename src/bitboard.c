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

// adds piece to bitboard on the given square
void addBitboard120(int sq, unsigned long long *bitboard) {
    int coord = SQ120SQ64(sq);
    *bitboard |= (0x01UL << coord);
}

// removes piece from bitboard on the given square
void removeBitboard(int file, int rank, unsigned long long *bitboard) {
    int coord = FR2SQ64(file, rank);
    *bitboard &= ~(0x01UL << coord);
}

// removes piece from bitboard on the given square
void removeBitboard120(int sq, unsigned long long *bitboard) {
    int coord = SQ120SQ64(sq);
    *bitboard &= ~(0x01UL << coord);
}

// return true if piece is on square
int checkBitboard120(int sq, unsigned long long *bitboard) {

    // TODO: add check for offboard
    int sq64 = SQ120SQ64(sq);

    ULL mask = (1 << sq64);
    ULL set = (*bitboard >> sq64 & 1ULL);

    if (set) {
        return TRUE;
    }
    return FALSE;
}

// return true if piece is on square
int checkBitboard(int file, int rank, unsigned long long *bitboard) {
    int coord = FR2SQ120(file, rank);
    return checkBitboard120(coord, bitboard);
}
