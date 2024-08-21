#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

TT tt[TT_SIZE];

int main() {
    BOARD_STATE board;

    TT *tt = (TT *)calloc(TT_SIZE, sizeof(TT));
    init(&board);

    startUCI();

    free(tt);

    return 0;
}
