#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

PVENTRY hashtable[PVSIZE];

int main() {
    BOARD_STATE board;
    init(&board);

    // ULL ray = slidingRay[7][27];
    // printBitboard(ray);
    //
    // int forward = bitScanForward(ray);
    // int backward = bitScanReverse(ray);

    // forward: N NE E SE 0-3
    // backward: S SW W NW 4-7

    // printf("f: %d, r: %d\n",forward,backward);

    PVENTRY *hashtable = (PVENTRY *)calloc(PVSIZE, sizeof(PVENTRY));

    startUCI();

    free(hashtable);

    return 0;
}
