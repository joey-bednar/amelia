#include "defs.h"
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

ULL zobrist_vals[12][64];
ULL zobristB2M;

int pawnOffset[2][4] = {{N, 2 * N, NW, NE}, {S, 2 * S, SW, SE}};

int inputDepth;
int inputTime[2];
int inputInc[2];

const int rookOffset[4] = {-10, -1, 10, 1};
const int bishopOffset[4] = {-11, -9, 9, 11};
const int promoteTo[4] = {bbQueen, bbKnight, bbRook, bbBishop};

int pawnSqTable[2][64];
int knightSqTable[2][64];
int bishopSqTable[2][64];
int rookSqTable[2][64];
int queenSqTable[2][64];
int kingSqTable[2][64];

ULL slidingRay[8][64];

static ULL initSingleSlidingRay(int sq64, int dir) {
    ULL bb = 0ull;
    int sq120 = SQ64SQ120(sq64);

    sq120 += dir;
    while (ONBOARD(sq120)) {
        int sq64now = SQ120SQ64(sq120);
        SETBIT(bb, sq64now);
        sq120 += dir;
    }
    return bb;
}

static void initSlidingRays() {

    const int dirs[8] = {N, NE, E, SE, S, SW, W, NW};
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 64; ++j) {
            slidingRay[i][j] = initSingleSlidingRay(j, dirs[i]);
        }
    }
}

static void initPieceSqMaps() {

    // clang-format off
    static const int pawn[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
    };
    static const int pawnEndgame[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
    };
    // static const int pawnEndgame[] = {
    // 0,  0,  0,  0,  0,  0,  0,  0,
    // 90, 90, 90, 90, 90, 90, 90, 90,
    // 60, 60, 60, 60, 60, 60, 60, 60,
    // 40, 40, 40, 40, 40, 40, 40, 40,
    // 20, 20, 20, 20, 20, 20, 20, 20,
    // 10, 10, 10, 10, 10, 10, 10, 10,
    //  5,  5,  5,  5,  5,  5,  5,  5,
    //  0,  0,  0,  0,  0,  0,  0,  0
    // };
    static const int knight[] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
    };
    static const int bishop[] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
    };
    static const int rook[] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
    };
    static const int queen[] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
    };
    static const int king[] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
    };
    static const int kingEndgame[] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
    };
    // static const int kingEndgame[] = {
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    //  0,  0,  0,  0,  0,  0,  0,  0,
    // };
    // clang-format on

    // rotate piece maps to match bitboard indices
    for (int i = 0; i < 64; ++i) {
        pawnSqTable[0][BBROTATE(i)] = pawn[i];
        knightSqTable[0][BBROTATE(i)] = knight[i];
        bishopSqTable[0][BBROTATE(i)] = bishop[i];
        rookSqTable[0][BBROTATE(i)] = rook[i];
        queenSqTable[0][BBROTATE(i)] = queen[i];
        kingSqTable[0][BBROTATE(i)] = king[i];

        pawnSqTable[1][BBROTATE(i)] = pawnEndgame[i];
        knightSqTable[1][BBROTATE(i)] = knight[i];
        bishopSqTable[1][BBROTATE(i)] = bishop[i];
        rookSqTable[1][BBROTATE(i)] = rook[i];
        queenSqTable[1][BBROTATE(i)] = queen[i];
        kingSqTable[1][BBROTATE(i)] = kingEndgame[i];
    }

    // for(int i=0;i<64;i++) {
    //     if(i % 8 == 0) {
    //         printf("\n");
    //     }
    //     printf("%d ",king[i]);
    // }
}

// array map for possible en passant squares
static void initEnpassantMap() {
    for (int i = 0; i < 120; ++i) {
        epMap[i] = FALSE;
    }

    for (int file = FILE_A; file <= FILE_H; ++file) {
        epMap[FR2SQ120(file, RANK_3)] = TRUE;
        epMap[FR2SQ120(file, RANK_6)] = TRUE;
    }
}

// convertion arrays for piece->color, and piece->opposite color
static void initColorMap() {
    for (int i = EMPTY; i <= OFFBOARD; ++i) {
        if (i >= wP && i <= wK) {
            colorMap[i] = WHITE;
            notcolorMap[i] = BLACK;
        } else if (i >= bP && i <= bK) {
            colorMap[i] = BLACK;
            notcolorMap[i] = WHITE;
        } else {
            colorMap[i] = BOTH;
            notcolorMap[i] = BOTH;
        }
    }
}

// conversion array from 120->64
static void initSqMap() {
    // junk value for squares not on 64sq board
    for (int i = 0; i < 120; ++i) {
        sq120sq64Map[i] = 69;
    }

    for (int i = 0; i < 64; ++i) {
        sq64sq120Map[i] = OFFBOARD;
    }

    for (int i = 0; i < 120; ++i) {
        onboardMap[i] = FALSE;
    }

    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        for (int file = FILE_A; file <= FILE_H; ++file) {
            int sq120 = FR2SQ120(file, rank);
            int sq64 = FR2SQ64(file, rank);
            sq120sq64Map[sq120] = sq64;
            sq64sq120Map[sq64] = sq120;
            onboardMap[sq120] = TRUE;
        }
    }
}

// conversion arrays for piece->color, and piece->opposite color
static void initPieceGenericMap() {
    genericMap[EMPTY] = EMPTY;
    genericMap[OFFBOARD] = OFFBOARD;
    genericMap[wP] = bbPawn;
    genericMap[wN] = bbKnight;
    genericMap[wB] = bbBishop;
    genericMap[wR] = bbRook;
    genericMap[wQ] = bbQueen;
    genericMap[wK] = bbKing;

    genericMap[bP] = bbPawn;
    genericMap[bN] = bbKnight;
    genericMap[bB] = bbBishop;
    genericMap[bR] = bbRook;
    genericMap[bQ] = bbQueen;
    genericMap[bK] = bbKing;

    toColor[WHITE][EMPTY] = EMPTY;
    toColor[WHITE][OFFBOARD] = OFFBOARD;
    toColor[WHITE][bbPawn] = wP;
    toColor[WHITE][bbKnight] = wN;
    toColor[WHITE][bbBishop] = wB;
    toColor[WHITE][bbRook] = wR;
    toColor[WHITE][bbQueen] = wQ;
    toColor[WHITE][bbKing] = wK;

    toColor[BLACK][EMPTY] = EMPTY;
    toColor[BLACK][OFFBOARD] = OFFBOARD;
    toColor[BLACK][bbPawn] = bP;
    toColor[BLACK][bbKnight] = bN;
    toColor[BLACK][bbBishop] = bB;
    toColor[BLACK][bbRook] = bR;
    toColor[BLACK][bbQueen] = bQ;
    toColor[BLACK][bbKing] = bK;
}

static void initJumps() {
    int offsetsKnight[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    int offsetsKing[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        for (int file = FILE_A; file <= FILE_H; ++file) {
            int sq120 = FR2SQ120(file, rank);
            int sq64 = FR2SQ64(file, rank);

            ULL bitboardKnight = 0;
            ULL bitboardKing = 0;
            for (int i = 0; i < 8; ++i) {
                int nextSqKnight = sq120 + offsetsKnight[i];
                int nextSqKing = sq120 + offsetsKing[i];
                if (ONBOARD(nextSqKnight)) {
                    SETBIT(bitboardKnight, SQ120SQ64(nextSqKnight));
                }

                if (ONBOARD(nextSqKing)) {
                    SETBIT(bitboardKing, SQ120SQ64(nextSqKing));
                }
            }
            knightJumps[sq64] = bitboardKnight;
            kingJumps[sq64] = bitboardKing;
        }
    }
}

void init(BOARD_STATE *board) {
    initEnpassantMap();
    initColorMap();
    initSqMap();
    initPieceGenericMap();
    initJumps();
    initPieceSqMaps();
    initZobrist();
    initSlidingRays();

    clearBoard(board);
    initBoard(board);
    loadZobrist(board);
}
