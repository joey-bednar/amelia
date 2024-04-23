#include "defs.h"
#include <assert.h>
#include <stdio.h>

int main() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    // init pawns
    board.pieces[WHITE] = 0;
    board.pieces[BLACK] = 0;
    board.pieces[BOTH] = 0;

    clearBoard(&board);

    initBoard(&board);
    printBoard(&board);
    printBits(board.pieces[BOTH]);

    generateMoves(&board, moves);

    return 0;
}
