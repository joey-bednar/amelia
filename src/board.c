#include "defs.h"
#include <stdio.h>
#include <assert.h>

// clears the chess board of any pieces
void clearBoard(BOARD_STATE *board) {
    // add offboard squares
    for (int i = 0; i < 120; i++) {
        board->board[i] = OFFBOARD;
    }

    // add onboard squares
    for (int file=FILE_A;file<=FILE_H;file++) {
        for (int rank=RANK_1;rank<=RANK_8;rank++) {
            board->board[FR2SQ120(file,rank)] = EMPTY;
        }
    }

    board->pieces[WHITE] = 0;
    board->pieces[BLACK] = 0;
    board->pieces[BOTH] = 0;

}

char getColor(char piece) {
    if (piece >= wP && piece <= wK) {
        return WHITE;
    } else if (piece >= bP && piece <= bK) {
        return BLACK;
    }
    return BOTH;
}

void setPiece(char piece,int file,int rank,BOARD_STATE *board) {

    // check for valid square
    assert(file >= FILE_A && file <= FILE_H);
    assert(rank >= RANK_1 && rank <= RANK_8);

    // check for valid piece
    assert(piece >= 0 && piece < MAX_PIECE);

    // set piece on board
    unsigned long sq = FR2SQ120(file,rank);
    board->board[sq] = piece;

    int coord = file+8*rank;

    // add/remove piece from shared bitboard
    unsigned long bit = (piece != EMPTY) & 0x01;
    board->pieces[BOTH] |= (bit << coord);
    printf("%d\n",coord);
    printBits(board->pieces[BOTH]);

}

// sets up the pieces for a new game
void initBoard(BOARD_STATE *board) {

    clearBoard(board);

    // add white pieces
    setPiece(wR,FILE_A,RANK_1,board);
    setPiece(wN,FILE_B,RANK_1,board);
    setPiece(wB,FILE_C,RANK_1,board);
    setPiece(wQ,FILE_D,RANK_1,board);
    setPiece(wK,FILE_E,RANK_1,board);
    setPiece(wB,FILE_F,RANK_1,board);
    setPiece(wN,FILE_G,RANK_1,board);
    setPiece(wR,FILE_H,RANK_1,board);

    // add black pieces
    setPiece(bR,FILE_A,RANK_8,board);
    setPiece(bN,FILE_B,RANK_8,board);
    setPiece(bB,FILE_C,RANK_8,board);
    setPiece(bQ,FILE_D,RANK_8,board);
    setPiece(bK,FILE_E,RANK_8,board);
    setPiece(bB,FILE_F,RANK_8,board);
    setPiece(bN,FILE_G,RANK_8,board);
    setPiece(bR,FILE_H,RANK_8,board);

    // add pawns
    for (int file=FILE_A;file<=FILE_H;file++) {
        setPiece(wP,file,RANK_2,board);
        setPiece(bP,file,RANK_7,board);
    }
}

// prints the pieces on the board
void printBoard(BOARD_STATE *board) {
    // char to represent a piece in FEN notation
    const int piece2char[] = {'.', 'P', 'N', 'B', 'R', 'Q', 'K',
                              'p', 'n', 'b', 'r', 'q', 'k'};
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            char display = piece2char[board->board[sq]];
            printf("%c ", display);
        }
        printf("\n");
    }
}
