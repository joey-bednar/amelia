#include "defs.h"
#include <assert.h>
#include <stdio.h>

// clears the chess board of any pieces
void clearBoard(BOARD_STATE *board) {
    // add offboard squares
    for (int i = 0; i < 120; i++) {
        board->board[i] = OFFBOARD;
    }

    // add onboard squares
    for (int file = FILE_A; file <= FILE_H; file++) {
        for (int rank = RANK_1; rank <= RANK_8; rank++) {
            board->board[FR2SQ120(file, rank)] = EMPTY;
        }
    }

    // remove kings
    board->kings[WHITE] = OFFBOARD;
    board->kings[BLACK] = OFFBOARD;

    // reset bit boards
    for (int i = 0; i <= bK; i++) {
        board->pieces[i] = 0;
    }

    board->turn = WHITE;
    board->fiftyMove = 0;
    board->castle = 0b1111;
    board->enpassant = OFFBOARD;
}

// return piece on square given by 120 index
int getPieceSq120(int sq, BOARD_STATE *board) {
    return board->board[sq];
}

// return piece on given square by file/rank
int getPieceFR(int file, int rank, BOARD_STATE *board) {
    int sq = FR2SQ120(file, rank);
    return getPieceSq120(sq, board);
}

void setPiece120(int piece, int sq, BOARD_STATE *board) {

    // // check for valid square/piece
    // int file = SQ120F(sq);
    // int rank = SQ120R(sq);
    // assert(file >= FILE_A && file <= FILE_H);
    // assert(rank >= RANK_1 && rank <= RANK_8);
    // assert(piece >= EMPTY && piece <= bK);

    // set piece on board
    board->board[sq] = piece;

    // remove piece from bitboards
    for (int i = EMPTY; i <= bK; i++) {
        removeBitboard120(sq, &board->pieces[i]);
    }

    // add piece to bitboard
    if (piece != EMPTY) {
        addBitboard120(sq, &board->pieces[piece]);
        addBitboard120(sq, &board->pieces[EMPTY]);
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

    // check for valid square
    assert(file >= FILE_A && file <= FILE_H);
    assert(rank >= RANK_1 && rank <= RANK_8);

    // check for valid piece
    assert(piece >= EMPTY && piece <= bK);

    // set piece on board
    int sq = FR2SQ120(file, rank);
    board->board[sq] = piece;

    // remove piece from bitboards
    for (int i = EMPTY; i <= bK; i++) {
        removeBitboard(file, rank, &board->pieces[i]);
    }

    // add piece to bitboard
    if (piece != EMPTY) {
        addBitboard(file, rank, &board->pieces[piece]);
        addBitboard(file, rank, &board->pieces[EMPTY]);
    }

    // update king placement
    if (piece == wK) {
        board->kings[WHITE] = sq;
    } else if (piece == bK) {
        board->kings[BLACK] = sq;
    }
}

// sets up the pieces for a new game
void initBoard(BOARD_STATE *board) {

    clearBoard(board);

    // add white pieces
    setPiece(wR, FILE_A, RANK_1, board);
    setPiece(wN, FILE_B, RANK_1, board);
    setPiece(wB, FILE_C, RANK_1, board);
    setPiece(wQ, FILE_D, RANK_1, board);
    setPiece(wK, FILE_E, RANK_1, board);
    setPiece(wB, FILE_F, RANK_1, board);
    setPiece(wN, FILE_G, RANK_1, board);
    setPiece(wR, FILE_H, RANK_1, board);

    // add black pieces
    setPiece(bR, FILE_A, RANK_8, board);
    setPiece(bN, FILE_B, RANK_8, board);
    setPiece(bB, FILE_C, RANK_8, board);
    setPiece(bQ, FILE_D, RANK_8, board);
    setPiece(bK, FILE_E, RANK_8, board);
    setPiece(bB, FILE_F, RANK_8, board);
    setPiece(bN, FILE_G, RANK_8, board);
    setPiece(bR, FILE_H, RANK_8, board);

    // add pawns
    for (int file = FILE_A; file <= FILE_H; file++) {
        setPiece(wP, file, RANK_2, board);
        setPiece(bP, file, RANK_7, board);
    }
}

// prints the pieces on the board
void printBoard(BOARD_STATE *board) {
    // int to represent a piece in FEN notation
    const int piece2char[] = {'.', 'P', 'N', 'B', 'R', 'Q', 'K',
                              'p', 'n', 'b', 'r', 'q', 'k'};
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            int display = piece2char[board->board[sq]];
            printf("%c ", display);
        }
        printf("\n");
    }
    printf("Turn: %d\n", board->turn);
    printf("En passant: %d\n", board->enpassant);
}

// prints the pieces on the board
void printBoardIndex(BOARD_STATE *board) {
    // int to represent a piece in FEN notation
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            printf("%d ", sq);
        }
        printf("\n");
    }
}

// array map for possible en passant squares
void initEnpassantMap(int *map) {
    for (int i = 0; i < 120; i++) {
        map[i] = FALSE;
    }

    for (int file = FILE_A; file <= FILE_H; file++) {
        map[FR2SQ120(file, RANK_3)] = TRUE;
        map[FR2SQ120(file, RANK_6)] = TRUE;
    }
}

// convertion arrays for piece->color, and piece->opposite color
void initColorMap(int *map, int *notmap) {
    for (int i = EMPTY; i <= OFFBOARD; i++) {
        if (i >= wP && i <= wK) {
            map[i] = WHITE;
            notmap[i] = BLACK;
        } else if (i >= bP && i <= bK) {
            map[i] = BLACK;
            notmap[i] = WHITE;
        } else {
            map[i] = BOTH;
            notmap[i] = BOTH;
        }
    }
}

// conversion array from 120->64
void initSqMap(int *sq120sq64Map, int *sq64sq120Map) {
    // junk value for squares not on 64sq board
    for (int i = 0; i < 120; i++) {
        sq120sq64Map[i] = 69;
    }

    for (int i = 0; i < 64; i++) {
        sq64sq120Map[i] = OFFBOARD;
    }

    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq120 = FR2SQ120(file, rank);
            int sq64 = FR2SQ64(file, rank);
            sq120sq64Map[sq120] = sq64;
            sq64sq120Map[sq64] = sq120;
        }
    }
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
    assert(bb != 0);
    return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}
