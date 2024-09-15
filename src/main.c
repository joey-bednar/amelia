#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

TT tt[TT_SIZE];

int main() {
    BOARD_STATE board;

    TT *tt = (TT *)calloc(TT_SIZE, sizeof(TT));
    init(&board);

    // loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    // 0 1", &board, 0); perft(6, &board);

    startUCI();

    free(tt);

    return 0;
}
