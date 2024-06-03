#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

static ULL perft(int depth, BOARD_STATE *board) {
    int n_moves, i;
    ULL nodes = 0;

    if (depth == 0) {
        return 1ULL;
    }

    MOVE moves[MAX_LEGAL_MOVES];

    n_moves = generateMoves(board, moves);

    for (i = 0; i < n_moves; i++) {
        // printf("start:\n");
        // printBoard(board);
        // printf("\n");
        if (isLegalMove(board, moves[i])) {

            // printf("depth %d:\n", depth);
            // printBoard(board);
            // printf("\n");

            makeMove(board, moves[i]);

            // printf("depth %d:\n", depth);
            // printBoard(board);
            // printf("\n================\n");
            nodes += perft(depth - 1, board);

            // printf("unmake:\n");
            // printBoard(board);
            // printf("\n");
            unmakeMove(board, moves[i]);
        }
    }

    return nodes;
}

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
    printf("map %d\n", epMap[23]);

    // setPiece(wK, FILE_E, RANK_1, &board);
    // setPiece(bK, FILE_E, RANK_8, &board);
    // setPiece(wP, FILE_E, RANK_5, &board);
    // setPiece(bP, FILE_D, RANK_7, &board);
    // board.turn = BLACK;
    // MOVE move;
    // move.startSquare = FR2SQ120(FILE_D, RANK_7);
    // move.endSquare = FR2SQ120(FILE_D, RANK_5);
    // move.captured = EMPTY;
    // move.epcapture = FALSE;
    // move.twopawnmove = TRUE;
    // move.castled = OFFBOARD;
    // move.priorep = OFFBOARD;
    // makeMove(&board, move);
    // printBoard(&board);

    clock_t t = clock();
    ULL num = perft(6, &board);
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("%lld nodes (%fs)\n", num, time_taken);

    printf("%lld\n", num + 258);

    // test();
    printBoardIndex(&board);

    return 0;
}
