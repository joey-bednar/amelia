#include "defs.h"
#include <assert.h>
#include <stdio.h>

static void testBitboards() {
    BOARD_STATE board;

    clearBoard(&board);
    for (int i = 0; i <= bK; i++) {
        assert(countBits(board.pieces[i]) == 0);
    }

    initBoard(&board);
    assert(countBits(board.pieces[EMPTY]) == 32);
    assert(countBits(board.pieces[wP]) == 8);
    assert(countBits(board.pieces[bP]) == 8);

    setPiece(wP, FILE_D, RANK_4, &board);
    assert(countBits(board.pieces[EMPTY]) == 33);

    setPiece(bP, FILE_E, RANK_5, &board);
    assert(countBits(board.pieces[EMPTY]) == 34);

    setPiece(bB, FILE_A, RANK_1, &board);
    assert(countBits(board.pieces[EMPTY]) == 34);
}

static void testBitboardIndex() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    setPiece(wK, FILE_A, RANK_1, &board);
    setPiece(wK, FILE_A, RANK_2, &board);
    setPiece(wK, FILE_H, RANK_8, &board);
    printBits(board.pieces[wK]);
    printf("\n");
    printBitboard(board.pieces[wK]);
}

static void testMoves() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    initBoard(&board);
    setPiece(wN, FILE_A, RANK_3, &board);
    setPiece(EMPTY, FILE_B, RANK_1, &board);
    printBoard(&board);

    generateMoves(&board, moves);

    printBoardIndex(&board);
}

static void testMakeMoves() {
    BOARD_STATE board;

    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    initBoard(&board);
    setPiece(wN, FILE_A, RANK_3, &board);
    setPiece(EMPTY, FILE_B, RANK_1, &board);
    generateMoves(&board, moves);

    printf("Root:\n");
    printBoard(&board);
    printf("\n");

    int i = 0;
    while (moves[i].startSquare != OFFBOARD) {
        printf("End:\n");
        makeMove(&board, moves[i]);
        printBoard(&board);
        printf("\n====================\n");
        printf("Restart:\n");
        unmakeMove(&board, moves[i]);
        printBoard(&board);
        printf("\n");
        i++;
    }

    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    clearBoard(&board);
    initBoard(&board);
    board.turn = BLACK;
    generateMoves(&board, moves);

    printf("Root:\n");
    printBoard(&board);
    printf("\n");

    i = 0;
    while (moves[i].startSquare != OFFBOARD) {
        printf("End:\n");
        makeMove(&board, moves[i]);
        printBoard(&board);
        printf("\n====================\n");
        printf("Restart:\n");
        unmakeMove(&board, moves[i]);
        printBoard(&board);
        printf("\n");
        i++;
    }

    printBoardIndex(&board);
}

static void printAllBoards() {
    BOARD_STATE board;

    // empty board
    clearBoard(&board);
    printBoard(&board);
    printf("\n");

    // starting board
    initBoard(&board);
    printBoard(&board);
    printf("\n");

    // board 120 index
    printBoardIndex(&board);
    printf("\n");

    // sq to file
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            char sq = FR2SQ120(file, rank);
            printf("%d%d ", SQ120F(sq), file);
        }
        printf("\n");
    }
    printf("\n");

    // sq to rank
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            char sq = FR2SQ120(file, rank);
            printf("%d%d ", SQ120R(sq), rank);
        }
        printf("\n");
    }
}

void test() {
    // testBitboardIndex();
    // testBitboards();
    // testMoves();
    testMakeMoves();
    // printAllBoards();
}
