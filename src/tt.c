#include "misc.h"
#include <assert.h>

#include "tt.h"

// clear table
void initTT() {
    for (int i = 0; i < TT_SIZE; ++i) {
        tt[i].hash = 0ull;
        tt[i].depth = 0;
        tt[i].flag = TT_EMPTY_FLAG;
        tt[i].best = 0ull;
        tt[i].val = 0;
    }
}

int probeTT(ULL hash, MOVE *best, int alpha, int beta, int depth) {
    TT *entry = &tt[hash % TT_SIZE];

    if (entry->hash != hash) {
        return TT_EMPTY;
    }

    if (entry->flag == TT_EMPTY_FLAG) {
        return TT_EMPTY;
    }

    if (entry->depth < depth) {
        return TT_EMPTY;
    }

    if (entry->flag == TT_EXACT_FLAG) {
        *best = entry->best;
        return entry->val;
    } else if (entry->flag == TT_ALPHA_FLAG && entry->val <= alpha) {
        *best = entry->best;
        return alpha;
    } else if (entry->flag == TT_BETA_FLAG && entry->val >= beta) {
        *best = entry->best;
        return beta;
    }

    return TT_EMPTY;
}

void storeTT(BOARD_STATE *board, MOVE best, int val, int flag, int depth) {

    int index = board->hash % TT_SIZE;

    tt[index].hash = board->hash;
    tt[index].depth = depth;
    tt[index].flag = flag;
    tt[index].best = best;
    tt[index].val = val;
}

MOVE getMoveTT(BOARD_STATE *board) {
    int index = board->hash % TT_SIZE;
    return tt[index].best;
}
