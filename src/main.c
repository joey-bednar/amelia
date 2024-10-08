#include "defs.h"
#include <stdlib.h>

TT *tt;

int main() {
    BOARD_STATE board;

    tt = (TT *)calloc(TT_SIZE, sizeof(TT));
    init(&board);

    startUCI();

    free(tt);

    return 0;
}
