#ifndef DEFS_H

#define FR2SQ120(f, r) ((21 + (f)) + ((r) * 10))
#define FR2SQ64(f, r) ((f) + ((r) * 8))
#define MAX_LEGAL_MOVES 256

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
// clang-format on

typedef unsigned long long ULL;

typedef struct {
    int board[120];

    unsigned long long pieces[3];

    int turn;
    int fiftyMove;
    int castle;
    int enpassant;
} BOARD_STATE;

typedef struct {
    int startSquare;
    int endSquare;
    int captured;
} MOVE;

enum { FALSE, TRUE };

// board.c
extern void initBoard(BOARD_STATE *board);
extern void clearBoard(BOARD_STATE *board);
extern void printBoard(BOARD_STATE *board);
extern void setPiece(char piece, int file, int rank, BOARD_STATE *board);
extern char getPieceFR(int file, int rank, BOARD_STATE *board);
extern char getPieceSq120(char sq, BOARD_STATE *board);

// bitboard.c
extern void clearBitboard(unsigned long long *bitboard);
extern void addBitboard(int file, int rank, unsigned long long *bitboard);
extern void removeBitboard(int file, int rank, unsigned long long *bitboard);
extern char checkBitboard(int file, int rank, unsigned long long *bitboard);

// moves.c
extern void generateMoves(BOARD_STATE *board, MOVE *move);

// utils.c
extern void printBits(unsigned long long num);
extern int countBits(unsigned long long b);
extern char getColor(char piece);

// test.c
extern void test();

#endif
