#include "defs.h"
#include <assert.h>
#include <stdio.h>

static ULL perft(int depth, BOARD_STATE *board) {
    int n_moves, i;
    ULL nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
        moves[i].captured = OFFBOARD;
    }

    n_moves = generateMoves(board, moves);

    for (i = 0; i < n_moves; i++) {
        // printf("start:\n");
        // printBoard(board);
        // printf("\n");

        makeMove(board, moves[i]);
        // printf("depth %d:\n",depth);
        // printBoard(board);
        // printf("\n");

        nodes += perft(depth - 1, board);

        // printf("unmake:\n");
        // printBoard(board);
        // printf("\n");
        unmakeMove(board, moves[i]);
    }

    return nodes;
}

int main() {
    BOARD_STATE board;
    board.turn = WHITE;
    board.castle = 0b1111;
    board.enpassant = 0;
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

    ULL num = perft(3, &board);

    printBoardIndex(&board);
    printf("%lld\n", num);

    // test();

    return 0;
}
