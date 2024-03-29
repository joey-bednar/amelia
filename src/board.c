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
            board->board[FR2SQ(file,rank)] = EMPTY;
        }
    }
}

void setPiece(char piece,int file,int rank,BOARD_STATE *board) {

    // check for valid square
    assert(file >= FILE_A && file <= FILE_H);
    assert(rank >= RANK_1 && rank <= RANK_8);

    // check for valid piece
    assert(piece >= 0 && piece < MAX_PIECE);

    board->board[FR2SQ(file,rank)] = piece;
}

// sets up the pieces for a new game
void initBoard(BOARD_STATE *board) {

    clearBoard(board);

    // add white pieces
    board->board[A1] = wR;
    board->board[B1] = wN;
    board->board[C1] = wB;
    board->board[D1] = wQ;
    board->board[E1] = wK;
    board->board[F1] = wB;
    board->board[G1] = wN;
    board->board[H1] = wR;

    // add black pieces
    board->board[A8] = bR;
    board->board[B8] = bN;
    board->board[C8] = bB;
    board->board[D8] = bQ;
    board->board[E8] = bK;
    board->board[F8] = bB;
    board->board[G8] = bN;
    board->board[H8] = bR;

    // add pawns
    for (int file=FILE_A;file<=FILE_H;file++) {
        board->board[FR2SQ(file,RANK_2)] = wP;
        board->board[FR2SQ(file,RANK_7)] = bP;
    }
}

// prints the pieces on the board
void printBoard(BOARD_STATE *board) {
    // char to represent a piece in FEN notation
    const int piece2char[] = {'.', 'P', 'N', 'B', 'R', 'Q', 'K',
                              'p', 'n', 'b', 'r', 'q', 'k'};
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ(file, rank);
            char display = piece2char[board->board[sq]];
            printf("%c ", display);
        }
        printf("\n");
    }
}
