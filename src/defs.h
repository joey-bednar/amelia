#ifndef DEFS_H
#define DEFS_H

// name
#define NAME "Amelia"
#define VERSION "0.15"
#define AUTHOR "Joey Bednar"

// settings
#define DEFAULTDEPTH 6
#define MAX_DEPTH 100
#define QMAXDEPTH 100
#define DEFAULT_TIME 3600000
#define DEFAULT_INC 0
#define ASPIRATION_WINDOW 50

// internal constants
#define FALSE 0
#define TRUE 1
#define MAX_LEGAL_MOVES 256
#define MAX_GAME_LENGTH 512
#define MATE 50000
#define MATETHRESHOLD 50
#define INF 99999

// squares
#define SQ120R(sq) (((sq) % 10) - 1)
#define SQ120F(sq) (((sq) - 21) / (10))
#define FR2SQ120(f, r) ((21 + (r)) + ((f) * 10))
#define FR2SQ64(f, r) ((r) + ((f) * 8))
#define SQ120SQ64(sq) (sq120sq64Map[(sq)])
#define SQ64SQ120(sq) (sq64sq120Map[(sq)])
#define CHAR2FILE(c) ((int)((c) - 'a'))
#define CHAR2RANK(c) ((int)((c) - '1'))
#define CHAR2SQ120(f, r) (FR2SQ120((CHAR2FILE((f))), (CHAR2RANK((r)))))

// pieces and colors
#define COLOR(p) (colorMap[(p)])
#define NOTCOLOR(p) (notcolorMap[(p)])
#define GENERIC(p) (genericMap[(p)])
#define TOWHITE(p) (toColor[WHITE][(p)])
#define TOBLACK(p) (toColor[BLACK][(p)])
#define TOCOLOR(c, p) (toColor[c][(p)])

// bit operations
#define CHECKBIT(bb, sq64) (((bb)) >> ((sq64)) & 1ULL)
#define CLEARBIT(bb, sq64) ((bb) &= ~(1ULL << sq64))
#define SETBIT(bb, sq64) ((bb) |= (1ULL << sq64))
#define CLEARBITBOARD(bb) ((bb) = 0)
#define BBROTATE(i) ((((i) >> 3) | ((i) << 3)) & 63) ^ 7

// math
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// attack bitboards
#define KNIGHTBB(sq64) (knightJumps[(sq64)])
#define KINGBB(sq64) (kingJumps[(sq64)])

#define BITLOOP(bb) for (; (bb); (bb) &= ((bb) - 1))

enum { NO_CASTLE, WK_CASTLE, WQ_CASTLE, BK_CASTLE, BQ_CASTLE, CASTLE_LENGTH };

typedef unsigned long long ULL;
typedef unsigned long long MOVE;


#endif
