#ifndef BOARD_H
#define BOARD_H

#include "misc.h"

#define MAX_DEPTH 100

// clang-format off
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, OFFBOARD };
enum { bbWhite, bbBlack, bbPawn, bbKnight, bbBishop, bbRook, bbQueen, bbKing, bbLength };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH, NONE };
enum {
  A1 = 21, A2, A3, A4, A5, A6, A7, A8,
  B1 = 31, B2, B3, B4, B5, B6, B7, B8,
  C1 = 41, C2, C3, C4, C5, C6, C7, C8,
  D1 = 51, D2, D3, D4, D5, D6, D7, D8,
  E1 = 61, E2, E3, E4, E5, E6, E7, E8,
  F1 = 71, F2, F3, F4, F5, F6, F7, F8,
  G1 = 81, G2, G3, G4, G5, G6, G7, G8,
  H1 = 91, H2, H3, H4, H5, H6, H7, H8,
};

enum {NW=-9, N=1, NE=11, E=10, SE=9, S=-1, SW=-11, W=-10};
enum { NO_CASTLE, WK_CASTLE, WQ_CASTLE, BK_CASTLE, BQ_CASTLE, CASTLE_LENGTH };
// clang-format on

// Saved position
typedef struct {
    ULL hash;
    int castle;
    int enpassant;
    int halfmove;
} POSRECORD;

typedef struct {
    ULL bitboard[bbLength];

    int turn;
    int halfmove;
    int fullmove;
    int castle;
    int enpassant;

    ULL hash;

    MOVE killers[MAX_DEPTH][2];
    int history[2][64][64];

    int ply;

    POSRECORD playedmoves[MAX_GAME_LENGTH * 2];
    int pmindex;

    int pvlength;
    MOVE pv[MAX_DEPTH];

    int ponder;
    long nodes;
    int stopped;
    int cutoffTime;
    float start;

} BOARD_STATE;

// Convert between 120, 64, file, rank
#define SQ120R(sq) (((sq) % 10) - 1)
#define SQ120F(sq) (((sq) - 21) / (10))
#define FR2SQ120(f, r) ((21 + (r)) + ((f) * 10))
#define FR2SQ64(f, r) ((r) + ((f) * 8))

#define SQ120SQ64(sq) (sq120sq64Map[(sq)])
extern int sq120sq64Map[120];

#define SQ64SQ120(sq) (sq64sq120Map[(sq)])
extern int sq64sq120Map[64];

// Square is on board
#define ONBOARD(sq) (onboardMap[(sq)])
extern int onboardMap[120];

// Check for color/piece
#define COLOR(p) (colorMap[(p)])
extern int colorMap[OFFBOARD + 1];

#define NOTCOLOR(p) (notcolorMap[(p)])
extern int notcolorMap[OFFBOARD + 1];

#define GENERIC(p) (genericMap[(p)])
extern int genericMap[OFFBOARD + 1];

#define TOWHITE(p) (toColor[WHITE][(p)])
#define TOBLACK(p) (toColor[BLACK][(p)])
#define TOCOLOR(c, p) (toColor[c][(p)])
extern int toColor[2][OFFBOARD + 1];








int getKingSq(BOARD_STATE *board, int color);

void setPiece(int piece, int file, int rank, BOARD_STATE *board);
void setPiece120(int piece, int sq, BOARD_STATE *board);

int getPieceSq120(int sq, BOARD_STATE *board);
int getGenericPieceSq120(int sq, BOARD_STATE *board);

int isEmptySquare(int sq, BOARD_STATE *board);

int getColorSq120(int sq, BOARD_STATE *board);

void clearPiece(BOARD_STATE *board, int piece, int sq);
void placePiece(BOARD_STATE *board, int piece, int sq);
void updateCastling(BOARD_STATE *board, MOVE move);

#endif
