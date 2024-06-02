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

    n_moves = generateMoves(board, moves);

    for (i = 0; i < n_moves; i++) {
        // printf("start:\n");
        // printBoard(board);
        // printf("\n");
        if (isLegalMove(board, moves[i])) {

            printf("depth %d:\n",depth);
            printBoard(board);
            printf("\n");

            makeMove(board, moves[i]);

            printf("depth %d:\n",depth);
            printBoard(board);
            printf("\n================\n");
            nodes += perft(depth - 1, board);


            // printf("unmake:\n");
            // printBoard(board);
            // printf("\n");
            unmakeMove(board, moves[i]);
        }
    }

    return nodes;
}


static int isAttackedSimple(BOARD_STATE *board, int sq, int *offsets, int sizeoffset, int enemycolor, int wKNP, int bKNP) {

    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        if (getColor(squareContains) == enemycolor) {
            if (squareContains == wKNP || squareContains == bKNP) {

                return TRUE;
            }
        }
    }

    return FALSE;
}

static int isAttackedSliding(BOARD_STATE *board, int sq, int *offsets, int sizeoffset, int enemycolor, int wRB,
                             int bRB) {
    int piece = getPieceSq120(sq, board);

    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];
        int squareContains = getPieceSq120(nextSq, board);

        while (squareContains == EMPTY) {
            nextSq = nextSq + offsets[i];
            squareContains = getPieceSq120(nextSq, board);

        }
        if (getColor(squareContains) == enemycolor) {

            if (squareContains == bQ || squareContains == wQ ||
                squareContains == wRB || squareContains == bRB) {
                return TRUE;
            }
        }
    }
    return FALSE;
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
    // setPiece(wK, FILE_A, RANK_8, &board);
    // setPiece(bK, FILE_C, RANK_8, &board);
    // setPiece(bN, FILE_B, RANK_3, &board);
    printBoard(&board);

    // int pawn[2] = {9, -11};
    // int king[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    // int knight[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    // int rook[4] = {-10, -1, 10, 1};
    // int bishop[4] = {-11, -9, 9, 11};
    //
    // int sum = 0;
    // for (int rank = RANK_8; rank >= RANK_1; rank--) {
    //     for (int file = FILE_A; file <= FILE_H; file++) {
    //         int sq = FR2SQ120(file, rank);
    //         // int attackedsimple = isAttackedSimple(&board, sq, pawn, 2, WHITE, wP, bP);
    //         int attackedsimple = isAttackedSliding(&board, sq, rook, 4, WHITE, wR, bR);
    //         int attacked = isAttacked(&board, sq, WHITE);
    //
    //         if (attackedsimple) {
    //             printf("%d is attacked simple: %d\n",sq,attackedsimple);
    //             sum++;
    //         }
    //
    //         if (attacked) {
    //             printf("%d is attacked: %d\n",sq,attacked);
    //         }
    //     }
    // }
    //
    // printf("sum: %d\n",sum);


    // ULL num = perft(1, &board);
    // printf("%lld\n", num);
    // num = perft(2, &board);
    // printf("%lld\n", num);
    // num = perft(3, &board);
    // printf("%lld\n", num);
    ULL num = perft(5, &board);
    printf("%lld\n", num);

    // test();
    printBoardIndex(&board);

    return 0;
}
