#ifndef DEFS_H

#define FALSE 0
#define TRUE 1

#define SQ120R(sq) (((sq) % 10) - 1)
#define SQ120F(sq) (((sq) - 21) / (10))
#define FR2SQ120(f, r) ((21 + (r)) + ((f) * 10))
#define FR2SQ64(f, r) ((r) + ((f) * 8))
#define MAX_LEGAL_MOVES 256

#define COLOR(p) (colorMap[(p)])
#define NOTCOLOR(p) (notcolorMap[(p)])

// clang-format off
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, OFFBOARD };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH, NONE };
enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8,
};
enum {NW=-9, N=1, NE=11, E=10, SE=9, S=-1, SW=-11, W=-10};
// clang-format on

enum { NO_CASTLE, WK_CASTLE, WQ_CASTLE, BK_CASTLE, BQ_CASTLE };

typedef unsigned long long ULL;

typedef struct {
    int board[120];

    unsigned long long pieces[13];

    int kings[2];

    int turn;
    int fiftyMove;
    int castle;
    int enpassant;
} BOARD_STATE;

typedef struct {
    int startSquare;
    int endSquare;
    int captured;
    int epcapture;
    int twopawnmove;
    int castled;
    int priorep;
} MOVE;

extern int epMap[120];
extern int colorMap[OFFBOARD + 1];
extern int notcolorMap[OFFBOARD + 1];

// board.c
extern void initBoard(BOARD_STATE *board);
extern void clearBoard(BOARD_STATE *board);
extern void printBoard(BOARD_STATE *board);
extern void printBoardIndex(BOARD_STATE *board);
extern void setPiece(int piece, int file, int rank, BOARD_STATE *board);
extern int getPieceFR(int file, int rank, BOARD_STATE *board);
extern int getPieceSq120(int sq, BOARD_STATE *board);

// bitboard.c
extern void clearBitboard(unsigned long long *bitboard);
extern void addBitboard(int file, int rank, unsigned long long *bitboard);
extern void removeBitboard(int file, int rank, unsigned long long *bitboard);
extern int checkBitboard(int file, int rank, unsigned long long *bitboard);

// moves.c
extern int generateMoves(BOARD_STATE *board, MOVE *move);
extern void makeMove(BOARD_STATE *board, MOVE move);
extern void unmakeMove(BOARD_STATE *board, MOVE move);
extern int isAttacked(BOARD_STATE *board, int sq, int enemycolor);
extern int isLegalMove(BOARD_STATE *board, MOVE move);

// utils.c
extern void printBits(unsigned long long num);
extern void printBitboard(unsigned long long num);
extern int countBits(unsigned long long b);
extern void initEnpassantMap(int *map);
extern void initColorMap(int *map, int *notmap);

// test.c
extern void test();

// perft.c
extern ULL perft(int depth);
extern ULL perft_rec(int depth, BOARD_STATE *board);
extern void printBenchmark(int depth);

#endif
