#include "defs.h"
#include <stdio.h>

// int eval(BOARD_STATE *board) {
//     ULL mine = board->bitboard[board->turn];
//     ULL yours = board->bitboard[!board->turn];
//
//     int total = 0;
//
//     const int val[] = {100, 320, 330, 500, 900, 20000};
//     for (int i = bbPawn; i < bbKing; i++) {
//         int me = countBits(mine & board->bitboard[i]);
//         int you = countBits(yours & board->bitboard[i]);
//         total += val[i - bbPawn] * (me - you);
//     }
//
//     return total;
// }

int eval(BOARD_STATE *board) {

    const int val[] = {100, 320, 330, 500, 900, 20000};
    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    while (mine != 0) {
        int index64 = bitScanForward(mine);
        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        // printBoard(board);
        // printf("index64: %d\n",index64);
        // printf("pst: %d\n",pawnSqTable[index64]);
        //
        ULL mask = (~1ULL << (index64));
        mine &= mask;

        if (board->turn == BLACK) {
            index64 = index64 ^ 7;
        }

        switch (piece) {
        case bbPawn:
            total += pawnSqTable[index64];
            break;
        case bbRook:
            total += rookSqTable[index64];
            break;
        case bbBishop:
            total += bishopSqTable[index64];
            break;
        case bbKnight:
            total += knightSqTable[index64];
            break;
        case bbQueen:
            total += queenSqTable[index64];
            break;
        case bbKing:
            total += kingSqTable[index64];
            break;
        }

        total += val[piece - bbPawn];
    }

    // printf("mine: %d\n",total);

    while (yours != 0) {
        int index64 = bitScanForward(yours);
        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        ULL mask = (~1ULL << (index64));
        yours &= mask;

        if (board->turn == WHITE) {
            index64 = index64 ^ 7;
        }

        switch (piece) {
        case bbPawn:
            total -= pawnSqTable[index64];
            break;
        case bbRook:
            total -= rookSqTable[index64];
            break;
        case bbBishop:
            total -= bishopSqTable[index64];
            break;
        case bbKnight:
            total -= knightSqTable[index64];
            break;
        case bbQueen:
            total -= queenSqTable[index64];
            break;
        case bbKing:
            total -= kingSqTable[index64];
            break;
        }

        total -= val[piece - bbPawn];
    }

    return total;
}

int negaMax(BOARD_STATE *board, int depth) {
    if (depth == 0) {
        return eval(board);
    }
    int max = -99999;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            // printf("Start:\n");
            // printBoard(board);
            makeMove(board, moves[i]);

            // printf("End:\n");
            // printBoard(board);
            int score = -negaMax(board, depth - 1);

            if (score > max) {
                max = score;
            }

            unmakeMove(board, moves[i]);
        }
    }

    return max;
}

MOVE makeBestMove(int depth, BOARD_STATE *board) {
    int max = -99999;
    MOVE best;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            makeMove(board, moves[i]);
            int score = -negaMax(board, depth);
            unmakeMove(board, moves[i]);
            if (score > 10000) {
                max = score;
                best = moves[i];
                // printf("(%d): move %d to %d\n", max);
                break;
            } else if (score > max) {
                max = score;
                best = moves[i];
                // printf("(%d): move %d to %d\n", max);
            }
        }
    }

    makeMove(board, best);
    return best;

    // printf("(%d): move %d to %d\n", max, startSq, endSq);
}

void printBestMove(int depth, BOARD_STATE *board) {
    MOVE best;

    int max = -99999;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            makeMove(board, moves[i]);
            int score = -negaMax(board, depth);
            unmakeMove(board, moves[i]);
            if (score > 10000) {
                best = moves[i];
                max = score;
                // printf("(%d): move %d to %d\n", max, startSq, endSq);
                break;
            } else if (score > max) {
                max = score;
                best = moves[i];
                // printf("(%d): move %d to %d\n", max, startSq, endSq);
            }
        }
    }

    char startFile = SQ120F(best.startSquare) + 'a';
    char startRank = SQ120R(best.startSquare) + '1';
    char endFile = SQ120F(best.endSquare) + 'a';
    char endRank = SQ120R(best.endSquare) + '1';

    char promote = '\0';
    if (best.type == MOVE_QUEENPROMOTE ||
        best.type == MOVE_QUEENPROMOTECAPTURE) {
        promote = 'q';
    } else if (best.type == MOVE_ROOKPROMOTE ||
               best.type == MOVE_ROOKPROMOTECAPTURE) {
        promote = 'r';
    } else if (best.type == MOVE_BISHOPPROMOTE ||
               best.type == MOVE_BISHOPPROMOTECAPTURE) {
        promote = 'b';
    } else if (best.type == MOVE_KNIGHTPROMOTE ||
               best.type == MOVE_KNIGHTPROMOTECAPTURE) {
        promote = 'k';
    }

    if (promote != '\0') {
        printf("bestmove %c%c%c%c%c\n", startFile, startRank, endFile, endRank,
               promote);
        // printf("%d %d\n",best.startSquare,best.endSquare);
    } else {
        printf("bestmove %c%c%c%c\n", startFile, startRank, endFile, endRank);
        // printf("%d %d\n",best.startSquare,best.endSquare);
    }
}
