#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

PVENTRY hashtable[PVSIZE];
TT tt[TT_SIZE];

int main() {
    BOARD_STATE board;
    init(&board);

    PVENTRY *hashtable = (PVENTRY *)calloc(PVSIZE, sizeof(PVENTRY));

    TT *tt = (TT *)calloc(TT_SIZE, sizeof(TT));

    startUCI();

    free(hashtable);
    free(tt);

    return 0;
}
