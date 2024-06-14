#include "defs.h"

int pawnSqTable[];
int knightSqTable[];
int bishopSqTable[];
int rookSqTable[];
int queenSqTable[];
int kingSqTable[];

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
    // clang-format on

    // rotate piece maps to match bitboard indices
    for (int i = 0; i < 64; i++) {
        pawnSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = pawn[i];
        knightSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = knight[i];
        bishopSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = bishop[i];
        rookSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = rook[i];
        queenSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = queen[i];
        kingSqTable[(((i >> 3) | (i << 3)) & 63) ^ 7] = king[i];
    }
}

// array map for possible en passant squares
static void initEnpassantMap() {
    for (int i = 0; i < 120; i++) {
        epMap[i] = FALSE;
    }

    for (int file = FILE_A; file <= FILE_H; file++) {
        epMap[FR2SQ120(file, RANK_3)] = TRUE;
        epMap[FR2SQ120(file, RANK_6)] = TRUE;
    }
}

// convertion arrays for piece->color, and piece->opposite color
static void initColorMap() {
    for (int i = EMPTY; i <= OFFBOARD; i++) {
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
    for (int i = 0; i < 120; i++) {
        sq120sq64Map[i] = 69;
    }

    for (int i = 0; i < 64; i++) {
        sq64sq120Map[i] = OFFBOARD;
    }

    for (int i = 0; i < 120; i++) {
        onboardMap[i] = FALSE;
    }

    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
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

    toWhite[EMPTY] = EMPTY;
    toWhite[OFFBOARD] = OFFBOARD;
    toWhite[bbPawn] = wP;
    toWhite[bbKnight] = wN;
    toWhite[bbBishop] = wB;
    toWhite[bbRook] = wR;
    toWhite[bbQueen] = wQ;
    toWhite[bbKing] = wK;

    toBlack[EMPTY] = EMPTY;
    toBlack[OFFBOARD] = OFFBOARD;
    toBlack[bbPawn] = bP;
    toBlack[bbKnight] = bN;
    toBlack[bbBishop] = bB;
    toBlack[bbRook] = bR;
    toBlack[bbQueen] = bQ;
    toBlack[bbKing] = bK;
}

static void initJumps() {
    int offsetsKnight[8] = {-21, 21, 19, -19, 8, -8, -12, 12};
    int offsetsKing[8] = {9, 10, 11, 1, -9, -10, -11, -1};
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int sq120 = FR2SQ120(file, rank);
            int sq64 = FR2SQ64(file, rank);

            ULL bitboardKnight = 0;
            ULL bitboardKing = 0;
            for (int i = 0; i < 8; i++) {
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

// clears the chess board of any pieces
void clearBoard(BOARD_STATE *board) {
    // remove kings
    board->kings[WHITE] = OFFBOARD;
    board->kings[BLACK] = OFFBOARD;

    for (int i = 0; i < bbLength; i++) {
        board->bitboard[i] = 0;
    }

    board->turn = WHITE;
    board->enpassant = OFFBOARD;
    board->halfmove = 0;
    board->fullmove = 0;
    SETBIT(board->castle, WK_CASTLE);
    SETBIT(board->castle, WQ_CASTLE);
    SETBIT(board->castle, BK_CASTLE);
    SETBIT(board->castle, BQ_CASTLE);
}

// sets up the pieces for a new game
void initBoard(BOARD_STATE *board) {

    clearBoard(board);

    // add white pieces
    setPiece(wR, FILE_A, RANK_1, board);
    setPiece(wN, FILE_B, RANK_1, board);
    setPiece(wB, FILE_C, RANK_1, board);
    setPiece(wQ, FILE_D, RANK_1, board);
    setPiece(wK, FILE_E, RANK_1, board);
    setPiece(wB, FILE_F, RANK_1, board);
    setPiece(wN, FILE_G, RANK_1, board);
    setPiece(wR, FILE_H, RANK_1, board);

    // add black pieces
    setPiece(bR, FILE_A, RANK_8, board);
    setPiece(bN, FILE_B, RANK_8, board);
    setPiece(bB, FILE_C, RANK_8, board);
    setPiece(bQ, FILE_D, RANK_8, board);
    setPiece(bK, FILE_E, RANK_8, board);
    setPiece(bB, FILE_F, RANK_8, board);
    setPiece(bN, FILE_G, RANK_8, board);
    setPiece(bR, FILE_H, RANK_8, board);

    // add pawns
    for (int file = FILE_A; file <= FILE_H; file++) {
        setPiece(wP, file, RANK_2, board);
        setPiece(bP, file, RANK_7, board);
    }
}

void init(BOARD_STATE *board) {
    initEnpassantMap();
    initColorMap();
    initSqMap();
    initPieceGenericMap();
    initJumps();
    initPieceSqMaps();

    clearBoard(board);
    initBoard(board);
}
