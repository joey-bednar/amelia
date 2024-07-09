#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

PVENTRY hashtable[PVSIZE];

int main() {
    BOARD_STATE board;
    init(&board);

    PVENTRY *hashtable = (PVENTRY *)calloc(PVSIZE, sizeof(PVENTRY));

    startUCI();

    free(hashtable);

    return 0;
}
