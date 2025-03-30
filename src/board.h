#ifndef BOARD_H
#define BOARD_H

#ifndef DEFS_H
#include "defs.h"
#endif

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
