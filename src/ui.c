#include "defs.h"
#include <stdio.h>

void playHuman(BOARD_STATE *board) {

    char moveString[32];
    int input = TRUE;
    while (TRUE) {

        printf("Input move: ");
        while (input) {
            scanf("%s", moveString);
            input = FALSE;
        }
        printf("Move: %s\n", moveString);

        MOVE moves[MAX_LEGAL_MOVES];
        int n_moves = generateMoves(board, moves);

        // char moveString[4] = "e2e4";

        int file = CHAR2FILE(moveString[0]);
        int rank = CHAR2RANK(moveString[1]);
        int start = FR2SQ120(file, rank);

        file = CHAR2FILE(moveString[2]);
        rank = CHAR2RANK(moveString[3]);
        int end = FR2SQ120(file, rank);

        int legal = FALSE;
        for (int i = 0; i < n_moves; i++) {
            if (moves[i].startSquare == start && moves[i].endSquare == end &&
                isLegalMove(board, moves[i])) {
                printf("Human played move %s\n", moveString);
                makeMove(board, moves[i]);
                printBoard(board);
                printf("\n\n\n\n");
                legal = TRUE;
            }
        }

        if (!legal) {
            printf("Illegal move!\n");
        } else {
            MOVE cpu = makeBestMove(3, board);
            char sfile = (char)(SQ120F(cpu.startSquare) + 'a');
            char srank = (char)(SQ120R(cpu.startSquare) + '1');
            char efile = (char)(SQ120F(cpu.endSquare) + 'a');
            char erank = (char)(SQ120R(cpu.endSquare) + '1');
            printf("Computer played move %c%c%c%c\n", sfile, srank, efile,
                   erank);
            printBoard(board);
            printf("\n\n\n\n");
        }

        input = TRUE;
    }
}
