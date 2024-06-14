#include "defs.h"
#include <stdio.h>

#define NEGINF -99999
#define POSINF 99999

// return the value of all pieces of a given color using piece values and square
// tables
static int computePieceTotals(ULL bb, int color, BOARD_STATE *board) {
    int total = 0;

    const int val[] = {100, 320, 330, 500, 900, 20000};

    while (bb != 0) {
        int index64 = bitScanForward(bb);
        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        ULL mask = (~1ULL << (index64));
        bb &= mask;

        if (color == BLACK) {
            index64 ^= 7;
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
    return total;
}

int eval(BOARD_STATE *board) {
    static int count = 0;
    count++;
    printf("count: %d\n", count);

    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    total += computePieceTotals(mine, board->turn, board);
    total -= computePieceTotals(yours, !board->turn, board);

    return total;
}

static int alphabeta(BOARD_STATE *board, int depth, int alpha, int beta) {
    int score = NEGINF;

    int legal = 0;

    if (depth == 0) {
        return eval(board);
    }

    if (board->halfmove >= 100) {
        return 0;
    }

    // TODO: add repetition check
    // TODO: add max depth check

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {

            legal++;
            makeMove(board, moves[i]);

            score = -alphabeta(board, depth - 1, -beta, -alpha);

            unmakeMove(board, moves[i]);

            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
    }

    if (legal == 0) {
        if (isAttacked(board, board->kings[board->turn], !board->turn)) {
            // checkmate
            return NEGINF;
        } else {
            // stalemate
            return 0;
        }
    }

    return alpha;
}

int negaMax(BOARD_STATE *board, int depth) {
    if (depth == 0) {
        return eval(board);
    }
    int max = NEGINF;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {

            makeMove(board, moves[i]);

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
            if (score > POSINF) {
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

    int score = NEGINF;
    int alpha = NEGINF;
    int beta = POSINF;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; i++) {
        if (isLegalMove(board, moves[i])) {
            makeMove(board, moves[i]);
            score = -alphabeta(board, depth - 1, -beta, -alpha);
            unmakeMove(board, moves[i]);

            if (score >= beta) {
                continue;
            }
            if (score > alpha) {
                alpha = score;
                best = moves[i];
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
