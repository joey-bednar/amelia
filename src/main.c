#include "defs.h"
#include <stdio.h>
#include <assert.h>

int main() {
    BOARD_STATE board;

    MOVE moves[MAX_LEGAL_MOVES];
    for (int i=0;i<MAX_LEGAL_MOVES;i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }
    moves[0].startSquare = E2;
    moves[0].endSquare = E4;

    initBoard(&board);
    printBoard(&board);

    generateMoves(&board,&moves[0]);
    /*for (int i=0;i<MAX_LEGAL_MOVES;i++) {*/
        /*if (moves[i].startSquare != OFFBOARD) {*/
            /*printf("%d, %d\n",moves[i].startSquare,moves[i].endSquare);*/
        /*}*/
    /*}*/
    return 0;
}
