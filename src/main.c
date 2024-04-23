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

    clearBoard(&board);
    /*initBoard(&board);*/

    /*test();*/

    setPiece(wK, FILE_E, RANK_1, &board);
    setPiece(wN, FILE_B, RANK_1, &board);
    printBoard(&board);

    generateMoves(&board, moves);

    return 0;
}
