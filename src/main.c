#include "defs.h"
#include <assert.h>
#include <stdio.h>

int epMap[120];
int sq120sq64Map[120];
int sq64sq120Map[64];
int colorMap[OFFBOARD + 1];
int notcolorMap[OFFBOARD + 1];
int genericMap[OFFBOARD + 1];
int toColor[2][OFFBOARD + 1];
int onboardMap[120];
ULL knightJumps[64];
ULL kingJumps[64];

int main() {
    BOARD_STATE board;
    init(&board);

    startUCI();
    // perft(6);
    // printBenchmark(6);

    // loadFEN("rnbqkbnr/1p2p3/8/p1pp1ppp/1PPPP1PP/8/P4P2/RNBQKBNR b KQkq - 0
    // 7", &board, 0);

    // ULL pawns = board.bitboard[bbPawn] & board.bitboard[board.turn];
    // printBitboard(0x8080808080808080);

    return 0;
}
