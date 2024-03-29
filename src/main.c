#include "defs.h"
#include <stdio.h>
#include <assert.h>


int main() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i=0;i<MAX_LEGAL_MOVES;i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    // init pawns
    board.pieces[WHITE] = 0;
    board.pieces[BLACK] = 0;
    board.pieces[BOTH] = 0;

    printBits(board.pieces[BOTH]);

    initBoard(&board);
    printBoard(&board);
    printBits(board.pieces[BOTH]);

    setPiece(EMPTY,FILE_A,RANK_1,&board);


    /*moves[0].startSquare = E2;*/
    /*moves[0].endSquare = E4;*/

    /*generateMoves(&board,&moves[0]);*/
    /*for (int i=0;i<MAX_LEGAL_MOVES;i++) {*/
        /*if (moves[i].startSquare != OFFBOARD) {*/
            /*printf("%d, %d\n",moves[i].startSquare,moves[i].endSquare);*/
        /*}*/
    /*}*/

    return 0;
}
