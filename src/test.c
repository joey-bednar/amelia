#include "defs.h"
#include <assert.h>
#include <stdio.h>

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
            int sq = FR2SQ120(file, rank);
            printf("%d%d ", SQ120F(sq), file);
        }
        printf("\n");
    }
    printf("\n");

    // sq to rank
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            printf("%d%d ", SQ120R(sq), rank);
        }
        printf("\n");
    }
}

static void testScan() {
    BOARD_STATE board;
    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq = FR2SQ120(file, rank);
            clearBoard(&board);
            setPiece120(wK, sq, &board);
            ULL all = (board.bitboard[bbWhite] | board.bitboard[bbBlack]);
            printBitboard(all);
            ULL bb = all;
            int result = SQ64SQ120(bitScanForward(bb));
            printf("result: %d\n", result);
            assert(result == sq);
        }
        printf("\n");
    }
}

static void testScanAll() {
    BOARD_STATE board;
    // init legal moves array
    MOVE moves[MAX_LEGAL_MOVES];
    for (int i = 0; i < MAX_LEGAL_MOVES; i++) {
        moves[i].startSquare = OFFBOARD;
        moves[i].endSquare = OFFBOARD;
    }

    initBoard(&board);

    ULL bb = (board.bitboard[bbWhite] | board.bitboard[bbBlack]);

    while (bb != 0) {
        printf("board:\n");
        printBitboard(bb);
        printBits(bb);
        printf("\n");

        int index64 = bitScanForward(bb);
        int result = SQ64SQ120(index64);
        printf("index: %d\n", result);

        printf("mask:\n");
        ULL mask = (~1ULL << (index64));
        printBitboard(mask);
        printBits(mask);
        printf("\n");
        bb &= mask;
    }
}

void test() {
    // testBitboardIndex();
    // testBitboards();
    // testMoves();
    // testMakeMoves();
    // printAllBoards();
    testScanAll();
}
