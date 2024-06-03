#include "defs.h"
#include <assert.h>
#include <stdio.h>

int epMap[120];

int main() {
    BOARD_STATE board;
    board.turn = WHITE;
    board.castle = 0b1111;
    board.enpassant = OFFBOARD;
    board.fiftyMove = 0;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
        moves[i].captured = OFFBOARD;
    }

    clearBoard(&board);
    initBoard(&board);
    printBoard(&board);

    initEnpassantMap(epMap);

    // for (int i=0;i<3;i++) {
    //     perft(i);
    // }
    perft(5);

    // test();
    printBoardIndex(&board);

    return 0;
}
