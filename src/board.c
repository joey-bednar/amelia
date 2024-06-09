#include "defs.h"
#include <assert.h>
#include <stdio.h>

int isEmptySquare(int sq, BOARD_STATE *board) {
    ULL bb = (board->bitboard[bbWhite] | board->bitboard[bbBlack]);
    ULL val = CHECKBIT(bb, SQ120SQ64(sq));

    return (ONBOARD(sq) && !val);
}

// TODO: fix
int hasEmptyEnemyPiece120(int sq, int color, BOARD_STATE *board) {
    ULL set = CHECKBIT(board->bitboard[color], SQ120SQ64(sq));
    return (ONBOARD(sq) && (!set));
}

// TODO: fix
int hasEnemyPiece120(int sq, BOARD_STATE *board) {
    ULL set = CHECKBIT(board->bitboard[NOTCOLOR(board->turn)], SQ120SQ64(sq));
    return (ONBOARD(sq) && set);
}

int getGenericPieceSq120(int sq, BOARD_STATE *board) {
    int sq64 = SQ120SQ64(sq);
    for (int i = bbPawn; i < bbAny; i++) {
        if (CHECKBIT(board->bitboard[i], sq64)) {
            return i;
        };
    }
    return -1;
}

int getColorSq120(int sq, BOARD_STATE *board) {
    ULL any = CHECKBIT(board->bitboard[bbAny], SQ120SQ64(sq));
    if (!any) {
        return BOTH;
    }
    ULL white = CHECKBIT(board->bitboard[bbWhite], SQ120SQ64(sq));
    if (white) {
        return WHITE;
    }
    return BLACK;
}

// return piece on square given by 120 index
int getPieceSq120(int sq, BOARD_STATE *board) {

    if (!ONBOARD(sq)) {
        return OFFBOARD;
    }

    int sq64 = SQ120SQ64(sq);

    ULL set = CHECKBIT(board->bitboard[bbAny], sq64);

    if (!set) {
        return EMPTY;
    }

    for (int i = bbPawn; i < bbAny; i++) {

        ULL val = CHECKBIT(board->bitboard[i], sq64);
        if (val) {
            if (CHECKBIT((board->bitboard[bbWhite]), sq64)) {
                return TOWHITE(i);
            }
            return TOBLACK(i);
        }
    }
}

// return piece on given square by file/rank
int getPieceFR(int file, int rank, BOARD_STATE *board) {
    int sq = FR2SQ120(file, rank);
    return getPieceSq120(sq, board);
}

void setPiece120(int piece, int sq, BOARD_STATE *board) {

    const int color = COLOR(piece);
    const int generic = GENERIC(piece);

    int sq64 = SQ120SQ64(sq);

    for (int i = 0; i < bbLength; i++) {
        CLEARBIT(board->bitboard[i], sq64);
    }

    // add piece to bitboard
    if (piece != EMPTY) {
        SETBIT(board->bitboard[generic], sq64);
        SETBIT(board->bitboard[color], sq64);
        SETBIT(board->bitboard[bbAny], sq64);
    }

    // update king placement
    if (piece == wK) {
        board->kings[WHITE] = sq;
    } else if (piece == bK) {
        board->kings[BLACK] = sq;
    }
}

// place piece on given square by updating board/bitboards
void setPiece(int piece, int file, int rank, BOARD_STATE *board) {

    int sq = FR2SQ120(file, rank);
    setPiece120(piece, sq, board);
}

// prints the pieces on the board
void printBoard(BOARD_STATE *board) {
    // int to represent a piece in FEN notation
    const int piece2char[] = {'.', 'P', 'N', 'B', 'R', 'Q', 'K',
                              'p', 'n', 'b', 'r', 'q', 'k'};
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            int display = piece2char[getPieceSq120(sq, board)];
            printf("%c ", display);
        }
        printf("\n");
    }
    printf("Turn: %d\n", board->turn);
    printf("En passant: %d\n", board->enpassant);
}

// prints the pieces on the board
void printBoardIndex() {
    printf("\n");
    // int to represent a piece in FEN notation
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            printf("%d ", sq);
        }
        printf("\n");
    }
    printf("\n");
}

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
