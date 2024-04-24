#include "defs.h"
#include <assert.h>
#include <stdio.h>

static void testBitboards() {
    BOARD_STATE board;

    clearBoard(&board);
    for (int i = 0; i <= bK; i++) {
        assert(countBits(board.pieces[i]) == 0);
    }

    initBoard(&board);
    assert(countBits(board.pieces[EMPTY]) == 32);
    assert(countBits(board.pieces[wP]) == 8);
    assert(countBits(board.pieces[bP]) == 8);

    setPiece(wP, FILE_D, RANK_4, &board);
    assert(countBits(board.pieces[EMPTY]) == 33);

    setPiece(bP, FILE_E, RANK_5, &board);
    assert(countBits(board.pieces[EMPTY]) == 34);

    setPiece(bB, FILE_A, RANK_1, &board);
    assert(countBits(board.pieces[EMPTY]) == 34);
}

static void testBitboardIndex() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    setPiece(wK, FILE_A, RANK_1, &board);
    setPiece(wK, FILE_A, RANK_2, &board);
    setPiece(wK, FILE_H, RANK_8, &board);
    printBits(board.pieces[wK]);
    printf("\n");
    printBitboard(board.pieces[wK]);
}

static void testMoves() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    setPiece(wK, FILE_E, RANK_1, &board);
    setPiece(wN, FILE_B, RANK_1, &board);
    printBoard(&board);

    generateMoves(&board, moves);

    printBits(board.pieces[EMPTY]);
    printBits(board.pieces[bK]);
}

void test() {
    testBitboardIndex();
    /*testBitboards();*/
    /*testMoves();*/
}
