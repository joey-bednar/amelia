#ifndef DEFS_H
#define DEFS_H

// internal constants
#define FALSE 0
#define TRUE 1
#define MAX_LEGAL_MOVES 256
#define MAX_GAME_LENGTH 512
#define MATE 50000
#define MATETHRESHOLD 50
#define INF 99999

// bit operations
#define CHECKBIT(bb, sq64) (((bb)) >> ((sq64)) & 1ULL)
#define CLEARBIT(bb, sq64) ((bb) &= ~(1ULL << sq64))
#define SETBIT(bb, sq64) ((bb) |= (1ULL << sq64))
#define CLEARBITBOARD(bb) ((bb) = 0)
#define BBROTATE(i) ((((i) >> 3) | ((i) << 3)) & 63) ^ 7

// math
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define BITLOOP(bb) for (; (bb); (bb) &= ((bb) - 1))

typedef unsigned long long ULL;
typedef unsigned long long MOVE;

#endif
