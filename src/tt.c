#include "defs.h"
#include <assert.h>

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

void storeTT(ULL hash, MOVE best, int val, int flag, int depth) {

    int index = hash % TT_SIZE;

    if (hash == tt[index].hash && depth < tt[index].depth) {
        return;
    }

    if (tt[index].hash == TT_EXACT_FLAG && hash != TT_EXACT_FLAG) {
        return;
    }

    tt[index].hash = hash;
    tt[index].depth = depth;
    tt[index].flag = flag;
    tt[index].best = best;
    tt[index].val = val;
}
