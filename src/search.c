#include "defs.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int searchDepth;

int compareMoves(const void *a, const void *b) {
    MOVE *moveA = (MOVE *)a;
    MOVE *moveB = (MOVE *)b;

    return (moveB->check - moveA->check) +
           (GENERIC(moveB->captured) - GENERIC(moveA->captured));
}

static void sortMoves(BOARD_STATE *board, MOVE *moves, int n_moves) {

    if (n_moves <= 1) {
        return;
    }

    if (hashtable[board->hash % PVSIZE].pos == board->hash) {
        for (int i = 0; i < n_moves; i++) {
            if (moves[i].startSquare ==
                    hashtable[board->hash % PVSIZE].move.startSquare &&
                moves[i].endSquare ==
                    hashtable[board->hash % PVSIZE].move.endSquare &&
                moves[i].promotion ==
                    hashtable[board->hash % PVSIZE].move.promotion) {
                MOVE temp = moves[i];
                moves[i] = moves[0];
                moves[0] = temp;
                // printf("pv used: ");
                // printMoveText(moves[0]);
                // printf("in %llu \n",board->hash);
            }
        }
        qsort(moves + 1, n_moves - 1, sizeof(MOVE), compareMoves);
        return;
    }
    qsort(moves, n_moves, sizeof(MOVE), compareMoves);
}

static int isMateEval(int score) {
    return ((score + MATETHRESHOLD >= MATE && score - MATETHRESHOLD <= MATE) ||
            (score + MATETHRESHOLD >= -MATE && score - MATETHRESHOLD <= -MATE));
}

// prints uci centipawn/mate search info
// if forced mate, return distance to mate
// otherwise return max depth
static int printEval(int score, int depth) {
    if (score + MATETHRESHOLD >= MATE && score - MATETHRESHOLD <= MATE) {
        int dist = (MATE - score) + depth;
        int mate = (dist + 1) / 2;
        printf("score mate %d ", mate);
        return dist;
    } else if (score + MATETHRESHOLD >= -MATE &&
               score - MATETHRESHOLD <= -MATE) {
        int dist = (MATE + score) + depth;
        int mate = (dist) / 2;
        printf("score mate -%d ", mate);
        return dist;
    } else {
        printf("score cp %d ", score);
        return MAX_DEPTH;
    }
}

// prints uci search info
static void printInfo(BOARD_STATE *board, double time, int score, int depth) {

    // print search info
    long nps = (long)floor(((double)1000 * board->nodes / time));
    long time_d = (long)time;

    printf("info ");
    printf("depth %d ", depth);
    printEval(score, depth);
    printf("nodes %ld ", (long)board->nodes);
    printf("nps %ld ", nps);
    printf("time %ld ", time_d);

    if (searchDepth == 0) {
        printf("\n");
        return;
    }

    printf("pv ");

    // pv length is always less than the search depth
    // pv length is cut short when mate is within depth
    int i = 0;
    // while (i < dist && i < searchDepth) {
    while (i < board->pvlength[0]) {
        printMoveText(board->pvarray[0][i]);
        printf(" ");
        i++;
    }
    printf("\n");
}

static int quiesce(BOARD_STATE *board, int depth, int alpha, int beta) {
    ++board->nodes;

    int score = -INF;

    int legal = 0;

    int stand_pat = eval(board);

    if (board->halfmove >= 100) {
        return 0;
    }

    if (isThreeFold(board)) {
        return 0;
    }

    if (isInsufficientMaterial(board)) {
        return 0;
    }

    if (depth == 0) {
        return stand_pat;
    }

    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    // sortMoves(board, moves, n_moves);
    qsort(moves, n_moves, sizeof(MOVE), compareMoves);

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, moves[i]);

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            ++legal;

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

    board->pvlength[searchDepth - depth] = searchDepth - depth;

    int score = -INF;

    int legal = 0;

    if (board->halfmove >= 100) {
        return 0;
    }

    if (isThreeFold(board)) {
        return 0;
    }

    if (isInsufficientMaterial(board)) {
        return 0;
    }

    if (depth == 0) {
        return quiesce(board, QMAXDEPTH, alpha, beta);
    }

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    // qsort(moves, n_moves, sizeof(MOVE), compareMoves);
    sortMoves(board, moves, n_moves);

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, moves[i]);

        if (!isAttacked(board, board->kings[!board->turn], board->turn)) {
            ++legal;
            score = -alphabeta(board, depth - 1, -beta, -alpha);
        }

        unmakeMove(board, moves[i]);

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;

            // add moves to pvarray
            int ply = searchDepth - depth;
            board->pvarray[ply][ply] = moves[i];
            for (int j = ply + 1; j < board->pvlength[ply + 1]; ++j) {
                board->pvarray[ply][j] = board->pvarray[ply + 1][j];
            }
            board->pvlength[ply] = board->pvlength[ply + 1];
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

static void copyPVtoTable(BOARD_STATE *board, int depth) {
    for (int i = 0; i < depth; i++) {
        ULL hash = board->hash;
        hashtable[hash % PVSIZE].pos = hash;
        hashtable[hash % PVSIZE].move = board->pvarray[0][i];
        // printf("stored ");
        // printMoveText(hashtable[hash % PVSIZE].move);
        // printf(" at %llu\n",board->hash);
        makeMove(board, hashtable[hash % PVSIZE].move);
        // printMoveText(pv.move);
    }

    for (int i = depth - 1; i >= 0; i--) {
        MOVE move = board->pvarray[0][i];
        // printMoveText(move);
        // printBoard(board);
        unmakeMove(board, move);
    }
    // printBoard(board);
}

static int searchCutoff(BOARD_STATE *board, double time_ms) {

    // bullet time control
    if (inputTime[board->turn] < 1000 * 60 * 1 && time_ms > 100) {
        return TRUE;
    }
    // blitz
    else if (inputTime[board->turn] < 1000 * 60 * 3 && time_ms > 250) {
        return TRUE;
    }
    // other
    else if (inputTime[board->turn] != DEFAULT_TIME && time_ms > 1000 * 1) {
        return TRUE;
    }
    return FALSE;
}

void search(BOARD_STATE *board) {

    // reset nodes searched
    board->nodes = 0;

    // begin timer
    clock_t t = clock();

    // iterative deepening
    MOVE bestmove;
    for (searchDepth = 0; searchDepth <= inputDepth; searchDepth++) {

        int score = alphabeta(board, searchDepth, -INF, INF);

        // compute time taken to search nodes
        clock_t new_t = clock() - t;
        double time_taken_ms = 1000 * ((double)new_t) / CLOCKS_PER_SEC;

        // print search info
        printInfo(board, time_taken_ms, score, searchDepth);

        // get bestmove/ponder from pv
        bestmove = board->pvarray[0][0];

        copyPVtoTable(board, board->pvlength[0]);

        // end searches in timed games if mate is found
        if (isMateEval(score) && inputTime[board->turn] != DEFAULT_TIME) {
            break;
        }

        // cutoff length of searches in timed games
        if (searchCutoff(board, time_taken_ms)) {
            break;
        }
    }

    // print best move
    printf("bestmove ");
    printMoveText(bestmove);
    printf("\n");
}
