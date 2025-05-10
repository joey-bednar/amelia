#ifndef BOARD_H
#define BOARD_H

#ifndef DEFS_H
#include "defs.h"
#endif

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
// clang-format on


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



#define ONBOARD(sq) (onboardMap[(sq)])


extern int epMap[120];
extern int sq120sq64Map[120];
extern int sq64sq120Map[64];
extern int colorMap[OFFBOARD + 1];
extern int notcolorMap[OFFBOARD + 1];
extern int genericMap[OFFBOARD + 1];

extern int toColor[2][OFFBOARD + 1];


extern int onboardMap[120];

extern int getKingSq(BOARD_STATE *board, int color);

extern void setPiece(int piece, int file, int rank, BOARD_STATE *board);
extern void setPiece120(int piece, int sq, BOARD_STATE *board);

extern int getPieceSq120(int sq, BOARD_STATE *board);
extern int getGenericPieceSq120(int sq, BOARD_STATE *board);

extern int isEmptySquare(int sq, BOARD_STATE *board);

extern int getColorSq120(int sq, BOARD_STATE *board);

extern void clearPiece(BOARD_STATE *board, int piece, int sq);
extern void placePiece(BOARD_STATE *board, int piece, int sq);
extern void updateCastling(BOARD_STATE *board, MOVE move);

#endif
