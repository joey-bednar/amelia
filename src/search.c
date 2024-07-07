#include "defs.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compareMoves(const void *moveA, const void *moveB) {
    return (CAPTURED(*(MOVE *)moveB) - CAPTURED(*(MOVE *)moveA));
}

static void sortMoves(BOARD_STATE *board, MOVE *moves, int n_moves) {

    if (n_moves <= 1) {
        return;
    }

    if (hashtable[board->hash % PVSIZE].pos == board->hash) {
        for (int i = 0; i < n_moves; i++) {
            if (START120(moves[i]) ==
                    START120(hashtable[board->hash % PVSIZE].move) &&
                END120(moves[i]) ==
                    END120(hashtable[board->hash % PVSIZE].move) &&
                PROMOTED(moves[i]) ==
                    PROMOTED(hashtable[board->hash % PVSIZE].move)) {
                MOVE temp = moves[i];
                moves[i] = moves[0];
                moves[0] = temp;
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
static void printEval(int score, int depth) {
    if (score + MATETHRESHOLD >= MATE && score - MATETHRESHOLD <= MATE) {
        int dist = (MATE - score) + depth;
        int mate = (dist + 1) / 2;
        printf("score mate %d ", mate);
    } else if (score + MATETHRESHOLD >= -MATE &&
               score - MATETHRESHOLD <= -MATE) {
        int dist = (MATE + score) + depth;
        int mate = (dist) / 2;
        printf("score mate -%d ", mate);
    } else {
        printf("score cp %d ", score);
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

    // skip pv output for search depth 0
    if (board->pvlength[0] == 0) {
        printf("\n");
        return;
    }

    printf("pv ");

    for (int i = 0; i < board->pvlength[0]; ++i) {
        printMoveText(board->pvarray[0][i]);
        printf(" ");
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
        ++board->ply;

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            ++legal;

            if (CAPTURED(moves[i]) != EMPTY) {
                score = -quiesce(board, depth - 1, -beta, -alpha);
            }
        }

        unmakeMove(board, moves[i]);
        --board->ply;

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    if (legal == 0) {
        if (isAttacked(board, SQ64SQ120(getKingSq(board, board->turn)),
                       !board->turn)) {
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

    board->pvlength[board->ply] = board->ply;

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
        ++board->ply;

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            ++legal;
            score = -alphabeta(board, depth - 1, -beta, -alpha);
        }

        unmakeMove(board, moves[i]);
        --board->ply;

        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;

            // add moves to pvarray
            board->pvarray[board->ply][board->ply] = moves[i];
            for (int j = board->ply + 1; j < board->pvlength[board->ply + 1];
                 ++j) {
                board->pvarray[board->ply][j] =
                    board->pvarray[board->ply + 1][j];
            }
            board->pvlength[board->ply] = board->pvlength[board->ply + 1];
        }
    }

    if (legal == 0) {
        if (isAttacked(board, SQ64SQ120(getKingSq(board, board->turn)),
                       !board->turn)) {
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

    char startFile = SQ120F(START120(move)) + 'a';
    char startRank = SQ120R(START120(move)) + '1';
    char endFile = SQ120F(END120(move)) + 'a';
    char endRank = SQ120R(END120(move)) + '1';

    char promote = '\0';
    if (PROMOTED(move) == bbQueen) {
        promote = 'q';
    } else if (PROMOTED(move) == bbRook) {
        promote = 'r';
    } else if (PROMOTED(move) == bbBishop) {
        promote = 'b';
    } else if (PROMOTED(move) == bbKnight) {
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
        makeMove(board, hashtable[hash % PVSIZE].move);
    }

    for (int i = depth - 1; i >= 0; i--) {
        MOVE move = board->pvarray[0][i];
        unmakeMove(board, move);
    }
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
    for (int searchDepth = 0; searchDepth <= inputDepth; searchDepth++) {

        board->ply = 0;

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
