#include "defs.h"
#include <assert.h>
#include <stdio.h>

static void testBitboards() {
    BOARD_STATE board;

    clearBoard(&board);
    assert(countBits(board.pieces[WHITE]) == 0);
    assert(countBits(board.pieces[BLACK]) == 0);
    assert(countBits(board.pieces[BOTH]) == 0);

    initBoard(&board);
    assert(countBits(board.pieces[WHITE]) == 16);
    assert(countBits(board.pieces[BLACK]) == 16);
    assert(countBits(board.pieces[BOTH]) == 32);

    setPiece(wP, FILE_D, RANK_4, &board);
    assert(countBits(board.pieces[WHITE]) == 17);
    assert(countBits(board.pieces[BLACK]) == 16);
    assert(countBits(board.pieces[BOTH]) == 33);

    setPiece(bP, FILE_E, RANK_5, &board);
    assert(countBits(board.pieces[WHITE]) == 17);
    assert(countBits(board.pieces[BLACK]) == 17);
    assert(countBits(board.pieces[BOTH]) == 34);

    setPiece(bB, FILE_A, RANK_1, &board);
    assert(countBits(board.pieces[WHITE]) == 16);
    assert(countBits(board.pieces[BLACK]) == 18);
    assert(countBits(board.pieces[BOTH]) == 34);
}

void test() {
    testBitboards();
}
