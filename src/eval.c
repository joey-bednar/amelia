#include "defs.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATE 50000
#define MATETHRESHOLD 50
#define INF 99999

static int isThreeFold(BOARD_STATE *board) {
    if (board->halfmove >= 4) {
        int end = 2 * (board->fullmove - 1) + board->turn;
        int start = 0; // end - board->halfmove;
        int count = 0;
        for (int i = start; i <= end; ++i) {
            if (board->hash == board->playedmoves[i]) {
                ++count;
            }
        }

        if (count >= 3) {
            return TRUE;
        }
    }

    return FALSE;
}

static void printEval(int score, int depth) {
    if (score + MATETHRESHOLD >= MATE && score - MATETHRESHOLD <= MATE) {
        int dist = (MATE - score) + depth;
        int mate = (dist + 1) / 2;
        printf("score mate %d ", mate);
    } else if (score + MATETHRESHOLD >= -MATE &&
               score - MATETHRESHOLD <= -MATE) {
        int dist = (-MATE - score) + depth;
        int mate = (dist + 1) / 2;
        printf("score mate -%d ", mate);
    } else {
        printf("score cp %d ", score);
    }
}

static void printInfo(BOARD_STATE *board, double time, int score, int depth) {

    // print search info
    long nps = (long)floor((double)(board->nodes / time));
    long time_d = (long)time;

    printf("info ");
    printf("depth %d ", DEFAULTDEPTH);
    printEval(score, DEFAULTDEPTH);
    printf("nodes %ld ", (long)board->nodes);
    printf("nps %ld ", nps);
    printf("time %ld ", time_d);

    printf("pv ");
    ULL key = board->hash % PVSIZE;
    for (int i = 0; i < DEFAULTDEPTH; ++i) {
        PVENTRY pv = board->pvtable[key];
        printMoveText(pv.move);
        printf(" ");
        key = pv.next;
    }
    printf("\n");
}

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
    ULL mine = board->bitboard[board->turn];
    ULL yours = board->bitboard[!board->turn];

    int total = 0;

    total += computePieceTotals(mine, board->turn, board);
    total -= computePieceTotals(yours, !board->turn, board);

    return total;
}

static int quiesce(BOARD_STATE *board, int depth, int alpha, int beta) {
    ++board->nodes;

    int score = -INF;

    int legal = 0;

    int stand_pat = eval(board);

    if (depth == 0) {
        return stand_pat;
    }

    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    if (board->halfmove >= 100) {
        return 0;
    }

    if (isThreeFold(board)) {
        return 0;
    }

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, moves[i]);

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            ++legal;

            assert(isLegalMove(board, moves[i]));

            if (moves[i].captured != EMPTY) {
                score = -quiesce(board, depth - 1, -beta, -alpha);
            }
        }

        unmakeMove(board, moves[i]);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    if (legal == 0) {
        if (isAttacked(board, board->kings[board->turn], !board->turn)) {
            // checkmate
            return -MATE - depth;
        } else {
            // stalemate
            return 0;
        }
    }

    return alpha;
}

static int alphabeta(BOARD_STATE *board, int depth, int alpha, int beta) {
    ++board->nodes;

    int score = -INF;

    int legal = 0;

    if (depth == 0) {
        return quiesce(board, QMAXDEPTH, alpha, beta);
        // return eval(board);
    }

    if (board->halfmove >= 100) {
        return 0;
    }

    if (isThreeFold(board)) {
        return 0;
    }

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, moves[i]);

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            ++legal;
            score = -alphabeta(board, depth - 1, -beta, -alpha);
        }

        ULL nextHash = board->hash;

        unmakeMove(board, moves[i]);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;

            board->line[(DEFAULTDEPTH - depth)] = moves[i];

            // add best move to pvtable
            board->pvtable[board->hash % PVSIZE].next = nextHash % PVSIZE;
            board->pvtable[board->hash % PVSIZE].move = moves[i];
            board->pvtable[board->hash % PVSIZE].depth = depth;
        }
    }

    if (legal == 0) {
        if (isAttacked(board, board->kings[board->turn], !board->turn)) {
            // checkmate
            return -MATE - depth;
        } else {
            // stalemate
            return 0;
        }
    }

    return alpha;
}

void printMoveText(MOVE move) {

    char startFile = SQ120F(move.startSquare) + 'a';
    char startRank = SQ120R(move.startSquare) + '1';
    char endFile = SQ120F(move.endSquare) + 'a';
    char endRank = SQ120R(move.endSquare) + '1';

    char promote = '\0';
    if (GENERIC(move.promotion) == bbQueen) {
        promote = 'q';
    } else if (GENERIC(move.promotion) == bbRook) {
        promote = 'r';
    } else if (GENERIC(move.promotion) == bbBishop) {
        promote = 'b';
    } else if (GENERIC(move.promotion) == bbKnight) {
        promote = 'n';
    }

    if (promote != '\0') {
        printf("%c%c%c%c%c", startFile, startRank, endFile, endRank, promote);
    } else {
        printf("%c%c%c%c", startFile, startRank, endFile, endRank);
    }
}

static int compare(const void *a, const void *b) {
    MOVE *moveA = (MOVE *)a;
    MOVE *moveB = (MOVE *)b;

    return (moveB->check - moveA->check) +
           (GENERIC(moveB->captured) - GENERIC(moveA->captured));
}

void printBestMove(int depth, BOARD_STATE *board) {
    int max = -INF;
    int alpha = -INF;
    int beta = INF;

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    // look at checks/captures first only for initial ply
    for (int i = 0; i < n_moves; ++i) {
        if (isCheck(board, moves[i])) {
            moves[i].check = 1;
        }
    }
    qsort(moves, n_moves, sizeof(MOVE), compare);

    // reset nodes searched
    board->nodes = 0;

    // begin timer
    clock_t t = clock();

    int score = alphabeta(board, DEFAULTDEPTH, alpha, beta);

    // compute time taken to search nodes
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    // print search info
    printInfo(board, time_taken, score, DEFAULTDEPTH);

    board->nodes = 0;

    // PVENTRY best = board->pvtable[board->hash % PVSIZE];
    // MOVE bestmove = board->pvtable[board->hash % PVSIZE].move;

    // use line variable until pvtable collisions fixed
    MOVE bestmove = board->line[(DEFAULTDEPTH - depth)];

    // print best move
    printf("bestmove ");
    printMoveText(bestmove);
    printf("\n");

    // printf("%d\n",best.depth);
}
