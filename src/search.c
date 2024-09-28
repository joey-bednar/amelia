#include "defs.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compareMoves(const void *moveA, const void *moveB) {

    MOVE A = *(MOVE *)moveA;
    MOVE B = *(MOVE *)moveB;

    return MVVLVA(PIECE(B), CAPTURED(B)) - MVVLVA(PIECE(A), CAPTURED(A));
}

static void scoreMoves(BOARD_STATE *board, MOVE *moves, int *scores,
                       int n_moves) {
    MOVE m;
    probeTT(board->hash, &m, INF, -INF, 0);

    for (int i = 0; i < n_moves; ++i) {
        if (moves[i] == m) {
            scores[i] = 9999;
        } else if (moves[i] == board->killers[board->ply][0]) {
            scores[i] = 50;
        } else if (moves[i] == board->killers[board->ply][1]) {
            scores[i] = 49;
        } else {
            scores[i] = MVVLVA(PIECE(moves[i]), CAPTURED(moves[i]));
        }
    }
}

static void pickSingleMoves(MOVE *moves, int *scores, int index, int n_moves) {
    int max = scores[index];
    int maxi = index;

    // find max score index
    for (int i = index; i < n_moves; ++i) {
        if (scores[i] > max) {
            max = scores[i];
            maxi = i;
        }
    }

    // move best move to front
    MOVE temp = moves[index];
    moves[index] = moves[maxi];
    moves[maxi] = temp;

    // move score to front
    int scoretemp = scores[index];
    scores[index] = scores[maxi];
    scores[maxi] = scoretemp;
}

static int isMateEval(int score) {
    return ((score + MATETHRESHOLD >= MATE && score - MATETHRESHOLD <= MATE) ||
            (score + MATETHRESHOLD >= -MATE && score - MATETHRESHOLD <= -MATE));
}

// prints uci centipawn/mate search info
static void printEval(int score) {

    if (score + MATETHRESHOLD >= MATE) {
        printf("score mate %d ", (MATE - score + 1) / 2);
    } else if (score - MATETHRESHOLD <= -MATE) {
        printf("score mate %d ", -(MATE + score) / 2);
    } else {
        printf("score cp %d ", score);
    }
}

static void printPV(BOARD_STATE *board, int depth) {
    MOVE list[depth];

    int i = 0;
    while (i < depth) {
        MOVE m;
        if (probeTT(board->hash, &m, INF, -INF, 0) == TT_EMPTY) {
            break;
        }
        printMoveText(m);
        printf(" ");
        makeMove(board, m);
        list[i++] = m;
    }

    while (i > 0) {
        unmakeMove(board, list[--i]);
    }
}

// prints uci search info
static void printInfo(BOARD_STATE *board, float time, int score, int depth) {

    // print search info
    long nps = floor((1000 * (double)board->nodes / time));

    printf("info ");
    printf("depth %d ", depth);
    MOVE m;
    int val = probeTT(board->hash, &m, INF, -INF, 0);
    printEval(score);
    printf("nodes %ld ", (long)board->nodes);
    printf("nps %ld ", nps);
    printf("time %ld ", (long)time);

    printf("pv ");
    printPV(board, depth);
    printf("\n");
}

static void checkTime(BOARD_STATE *board) {
    if (board->nodes % 1024 == 0) {
        float new_t = clock() - board->start;
        float time_taken_ms = (1000 * new_t) / CLOCKS_PER_SEC;
        if (time_taken_ms > board->cutoffTime) {
            board->stopped = TRUE;
        }
    }
}

static int quiesce(BOARD_STATE *board, int depth, int alpha, int beta) {
    checkTime(board);
    if (board->stopped) {
        return 0;
    }
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

    qsort(moves, n_moves, sizeof(MOVE), compareMoves);

    for (int i = 0; i < n_moves; ++i) {

        makeMove(board, moves[i]);
        ++board->ply;

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            ++legal;

            if (CAPTURED(moves[i]) != EMPTY || PROMOTED(moves[i]) != EMPTY) {
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
            return -MATE + board->ply;
        } else {
            // stalemate
            return 0;
        }
    }

    return alpha;
}

static int alphabeta(BOARD_STATE *board, int depth, int alpha, int beta,
                     int doNull) {

    checkTime(board);
    if (board->stopped) {
        return 0;
    }
    ++board->nodes;

    int score = -INF;
    int bestscore = -INF;
    int oldalpha = alpha;

    MOVE bestmove;
    int flag = TT_ALPHA_FLAG;

    int legal = 0;

    if (board->halfmove >= 100 || isThreeFold(board) ||
        isInsufficientMaterial(board)) {
        return 0;
    }

    // check extension
    int incheck = isAttacked(board, SQ64SQ120(getKingSq(board, board->turn)),
                             !board->turn);
    if (incheck) {
        ++depth;
    }

    if (depth == 0) {
        return quiesce(board, QMAXDEPTH, alpha, beta);
    }

    int val = probeTT(board->hash, &bestmove, alpha, beta, depth);
    if (val != TT_EMPTY) {
        return val;
    }

    // null move pruning
    if (doNull && depth >= 4 && !incheck) {

        makeNullMove(board);
        int nullScore = -alphabeta(board, depth - 2, -beta, -beta + 1, FALSE);
        unmakeNullMove(board);

        if (nullScore >= beta) {
            return beta;
        }
    }

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    int scores[n_moves];
    scoreMoves(board, moves, scores, n_moves);

    for (int i = 0; i < n_moves; ++i) {

        pickSingleMoves(moves, scores, i, n_moves);

        makeMove(board, moves[i]);
        ++board->ply;

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            ++legal;

            if (i == 0) {
                // full window search on pv node
                score = -alphabeta(board, depth - 1, -beta, -alpha, doNull);
            } else {
                // check if other moves score better than PV
                score =
                    -alphabeta(board, depth - 1, -alpha - 1, -alpha, doNull);

                // prevent re-search of non-PV nodes
                if (score > alpha && beta - alpha > 1) {
                    score = -alphabeta(board, depth - 1, -beta, -alpha, doNull);
                }
            }
        }

        unmakeMove(board, moves[i]);
        --board->ply;

        if (score > bestscore) {
            bestscore = score;
            bestmove = moves[i];
            if (score > alpha) {
                if (score >= beta && !board->stopped) {
                    bestmove = moves[i];

                    storeTT(board, moves[i], beta, TT_BETA_FLAG, depth);
                    if (CAPTURED(moves[i]) == EMPTY && doNull) {
                        board->killers[board->ply][1] =
                            board->killers[board->ply][0];
                        board->killers[board->ply][0] = moves[i];
                    }

                    return beta;
                }
            }
            flag = TT_EXACT_FLAG;
            alpha = score;
        }
    }

    if (legal == 0) {
        if (incheck) {
            // checkmate
            return -MATE + board->ply;
        } else {
            // stalemate
            return 0;
        }
    }

    if (alpha != oldalpha && !board->stopped) {
        storeTT(board, bestmove, bestscore, TT_EXACT_FLAG, depth);
    } else if (!board->stopped) {
        storeTT(board, bestmove, alpha, TT_ALPHA_FLAG, depth);
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

// returns true when next iteration of search should be
// prevented due to time restrictions
static int searchCutoff(BOARD_STATE *board, float time_ms) {

    int time = inputTime[board->turn];
    if (time == DEFAULT_TIME) {
        return FALSE;
    }

    // if used more than half of allocated time,
    // don't start a new search
    return (time_ms * 2 >= board->cutoffTime);
}

static float setCutoff(BOARD_STATE *board) {
    int time = inputTime[board->turn];
    int inc = inputInc[board->turn];

    if (time == DEFAULT_TIME) {
        return 1000 * 60 * 36;
    }

    return (time / 20) + (4 * inc / 5);
}

// returns TRUE if only one move is legal
// adds only move to "move"
static int onlyMove(BOARD_STATE *board, MOVE *move) {
    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    int legal = 0;
    int legalindex = 0;

    int i = 0;
    while (i < n_moves && legal < 2) {

        makeMove(board, moves[i]);

        if (!isAttacked(board, SQ64SQ120(getKingSq(board, !board->turn)),
                        board->turn)) {
            ++legal;
            legalindex = i;
        }

        unmakeMove(board, moves[i]);
        i++;
    }

    if (legal == 1) {
        *move = moves[legalindex];
        return TRUE;
    }
    return FALSE;
}

void search(BOARD_STATE *board) {

    MOVE bestmove;

    // reset nodes searched
    board->nodes = 0;

    // reset search time parameters
    board->stopped = FALSE;
    board->cutoffTime = setCutoff(board);

    // begin timer
    board->start = clock();

    // if only one legal move, play instantly
    if (onlyMove(board, &bestmove)) {
        // print best move
        printf("bestmove ");
        printMoveText(bestmove);
        printf("\n");
        return;
    }

    // iterative deepening
    int alpha = -INF;
    int beta = INF;
    for (int searchDepth = 1; searchDepth <= inputDepth; searchDepth++) {

        board->ply = 0;

        int score = alphabeta(board, searchDepth, alpha, beta, TRUE);

        if (board->stopped) {
            break;
        }

        // // redo search with INF window if score is outside aspiration
        // window if (score <= alpha || score >= beta) {
        //     alpha = -INF;
        //     beta = INF;
        //     searchDepth--;
        //     continue;
        // }
        //
        // // set aspiration window
        // alpha = score - ASPIRATION_WINDOW;
        // beta = score + ASPIRATION_WINDOW;

        // measure time spent
        float new_t = clock() - board->start;
        float time_taken_ms = (1000 * new_t) / CLOCKS_PER_SEC;

        // print search info
        printInfo(board, time_taken_ms, score, searchDepth);

        // get bestmove/ponder from pv
        // bestmove = board->pvarray[0][0];
        probeTT(board->hash, &bestmove, INF, -INF, 0);

        // prevent new searches if not enough time
        if (searchCutoff(board, time_taken_ms)) {
            break;
        }
    }

    // print best move
    printf("bestmove ");
    printMoveText(bestmove);
    printf("\n");

    initTT();
}
