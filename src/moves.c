#include "defs.h"
#include <assert.h>
#include <stdio.h>

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {

    switch (move.castled) {
    case NO_CASTLE:
        break;
    case WK_CASTLE:
        setPiece120(wR, F1, board);
        setPiece120(EMPTY, H1, board);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        break;
    case WQ_CASTLE:
        setPiece120(wR, D1, board);
        setPiece120(EMPTY, A1, board);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        break;
    case BK_CASTLE:
        setPiece120(bR, F8, board);
        setPiece120(EMPTY, H8, board);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    case BQ_CASTLE:
        setPiece120(bR, D8, board);
        setPiece120(EMPTY, A8, board);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    }

    // TODO: cleanup
    int piece = getPieceSq120(move.startSquare, board);

    setPiece120(EMPTY, move.startSquare, board);
    setPiece120(piece, move.endSquare, board);

    if (piece == wR && move.startSquare == H1) {
        CLEARBIT(board->castle, WK_CASTLE);
    } else if (piece == wR && move.startSquare == A1) {
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (piece == bR && move.startSquare == H8) {
        CLEARBIT(board->castle, BK_CASTLE);
    } else if (piece == bR && move.startSquare == A8) {
        CLEARBIT(board->castle, BQ_CASTLE);
    } else if (piece == wK && move.startSquare == E1) {
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (piece == bK && move.startSquare == E8) {
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
    }

    const int offset[2] = {S, N};

    if (move.twopawnmove) {
        board->enpassant = move.endSquare + offset[board->turn];
    } else {
        board->enpassant = OFFBOARD;
    }

    if (move.epcapture) {
        board->enpassant = OFFBOARD;
        setPiece120(EMPTY, move.endSquare + offset[board->turn], board);
    } else if (move.promotion != EMPTY) {
        setPiece120(move.promotion, move.endSquare, board);
    }
    board->turn = !(board->turn);
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {

    if (move.castled == WK_CASTLE) {
        setPiece120(EMPTY, F1, board);
        setPiece120(wR, H1, board);
    } else if (move.castled == WQ_CASTLE) {
        setPiece120(EMPTY, D1, board);
        setPiece120(wR, A1, board);
    } else if (move.castled == BK_CASTLE) {
        setPiece120(EMPTY, F8, board);
        setPiece120(bR, H8, board);
    } else if (move.castled == BQ_CASTLE) {
        setPiece120(EMPTY, D8, board);
        setPiece120(bR, A8, board);
    }
    board->castle = move.priorcastle;

    // TODO: cleanup
    int offset = S;
    if (board->turn == WHITE) {
        offset = N;
    }

    int piece = getPieceSq120(move.endSquare, board);

    if (move.epcapture) {
        board->enpassant = move.endSquare;
        setPiece120(move.captured, move.endSquare + offset, board);
        setPiece120(EMPTY, move.endSquare, board);
        setPiece120(piece, move.startSquare, board);
        board->turn = !(board->turn);
        return;
    }

    setPiece120(move.captured, move.endSquare, board);
    if (move.promotion == EMPTY) {
        setPiece120(piece, move.startSquare, board);
    } else {
        int pawn = wP;
        if (COLOR(piece) == BLACK) {
            pawn = bP;
        }
        setPiece120(pawn, move.startSquare, board);
    }

    if (move.twopawnmove) {
        board->enpassant = move.priorep;
    }

    board->turn = !(board->turn);
}

// add a move to the provided moves array
static void addMove(BOARD_STATE *board, MOVE *moves, int start, int end,
                    int captured, int epcapture, int twopawnmove, int castled,
                    int promotion, int *index) {
    // moves[*index].piece = piece;
    moves[*index].startSquare = start;
    moves[*index].endSquare = end;
    moves[*index].captured = captured;
    moves[*index].epcapture = epcapture;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castled = castled;
    moves[*index].promotion = promotion;
    moves[*index].priorep = board->enpassant;
    moves[*index].priorcastle = board->castle;
    (*index)++;
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, int *offsets, int offsetssize) {

    for (int i = 0; i < offsetssize; i++) {
        int nextSq = sq + offsets[i];

        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        while (isEmptySquare(nextSq, board)) {
            addMove(board, moves, sq, nextSq, EMPTY, FALSE, FALSE, NO_CASTLE,
                    EMPTY, index);

            nextSq = nextSq + offsets[i];
        }

        if (ONBOARD(nextSq) &&
            CHECKBIT(board->bitboard[(!board->turn)], SQ120SQ64(nextSq))) {
            int squareContains = getPieceSq120(nextSq, board);
            addMove(board, moves, sq, nextSq, squareContains, FALSE, FALSE,
                    NO_CASTLE, EMPTY, index);
        }
    }
}

static void generateCastleMoves(BOARD_STATE *board, MOVE *moves, int *index) {

    static int castles = 0;

    int rank = RANK_1;
    int queenside = WQ_CASTLE;
    int kingside = WK_CASTLE;
    int rook = wR;
    if (board->turn == BLACK) {
        rank = RANK_8;
        queenside = BQ_CASTLE;
        kingside = BK_CASTLE;
        rook = bR;
    }

    int color = board->turn;

    if (isAttacked(board, board->kings[color], !color)) {
        return;
    }

    if (board->kings[color] == FR2SQ120(FILE_E, rank)) {
        if (CHECKBIT(board->castle, kingside) &&
            getPieceSq120(FR2SQ120(FILE_H, rank), board) == rook) {

            int throughcheck =
                isAttacked(board, FR2SQ120(FILE_F, rank), !color) ||
                isAttacked(board, FR2SQ120(FILE_G, rank), !color);

            int blocked =
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_F, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_G, rank));

            if (!throughcheck && !blocked) {
                addMove(board, moves, FR2SQ120(FILE_E, rank),
                        FR2SQ120(FILE_G, rank), EMPTY, FALSE, FALSE, kingside,
                        EMPTY, index);
            }
        }
        if (CHECKBIT(board->castle, queenside) &&
            getPieceSq120(FR2SQ120(FILE_A, rank), board) == rook) {

            int throughcheck =
                isAttacked(board, FR2SQ120(FILE_C, rank), !color) ||
                isAttacked(board, FR2SQ120(FILE_D, rank), !color);

            int blocked =
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_B, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_C, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_D, rank));

            if (!throughcheck && !blocked) {
                addMove(board, moves, FR2SQ120(FILE_E, rank),
                        FR2SQ120(FILE_C, rank), EMPTY, FALSE, FALSE, queenside,
                        EMPTY, index);
            }
        }
    }
}

static void addPromotions(BOARD_STATE *board, MOVE *moves, int start, int end,
                          int captured, int color, int *index) {
    int promoteTo[4] = {wQ, wN, wR, wB};
    if (color == BLACK) {
        promoteTo[0] = bQ;
        promoteTo[1] = bN;
        promoteTo[2] = bR;
        promoteTo[3] = bB;
    }
    for (int i = 0; i < 4; i++) {
        addMove(board, moves, start, end, captured, FALSE, FALSE, FALSE,
                promoteTo[i], index);
    }
}

static void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int color = board->turn;

    int secondrank = RANK_2;
    int eighthrank = RANK_8;
    int offset[4] = {1, 2, -9, 11};
    int enemypawn = bP;
    if (color == BLACK) {
        offset[0] = -1;
        offset[1] = -2;
        offset[2] = 9;
        offset[3] = -11;
        secondrank = RANK_7;
        eighthrank = RANK_1;
        enemypawn = wP;
    }

    int one = sq + offset[0];
    int two = sq + offset[1];

    // up one
    if (isEmptySquare(one, board)) {
        if (SQ120R(one) == eighthrank) {
            addPromotions(board, moves, sq, one, EMPTY, color, index);
        } else {
            addMove(board, moves, sq, one, EMPTY, FALSE, FALSE, NO_CASTLE,
                    EMPTY, index);
        }
    }

    // captures and en passant
    for (int i = 2; i <= 3; i++) {
        int enemysquare = sq + offset[i];

        if (!ONBOARD(enemysquare)) {
            continue;
        }

        if (CHECKBIT(board->bitboard[(!board->turn)], SQ120SQ64(enemysquare))) {
            int enemypiece = getPieceSq120(enemysquare, board);

            if (SQ120R(enemysquare) == eighthrank) {
                addPromotions(board, moves, sq, enemysquare, enemypiece, color,
                              index);
            } else {
                addMove(board, moves, sq, enemysquare, enemypiece, FALSE, FALSE,
                        NO_CASTLE, EMPTY, index);
            }
        } else if (epMap[enemysquare] && enemysquare == board->enpassant) {

            addMove(board, moves, sq, enemysquare, enemypawn, TRUE, FALSE,
                    NO_CASTLE, EMPTY, index);
        }
    }

    // up two
    if (SQ120R(sq) == secondrank && isEmptySquare(one, board) &&
        isEmptySquare(two, board)) {
        addMove(board, moves, sq, two, EMPTY, FALSE, TRUE, NO_CASTLE, EMPTY,
                index);
    }
}

static void generatePseudoPresetMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      ULL bitboard, int *index) {
    int sq64 = SQ120SQ64(sq);
    ULL bb = ~board->bitboard[board->turn] & bitboard;

    while (bb != 0) {
        int nextSq64 = bitScanForward(bb);
        int nextSq120 = SQ64SQ120(nextSq64);

        int squareContains = getPieceSq120(nextSq120, board);
        addMove(board, moves, sq, nextSq120, squareContains, FALSE, FALSE,
                NO_CASTLE, EMPTY, index);

        ULL mask = (~1ULL << (nextSq64));
        bb &= mask;
    }
}

static void generatePseudoKingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    generatePseudoPresetMoves(board, moves, sq, KINGBB(SQ120SQ64(sq)), index);
}

static void generatePseudoKnightMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {

    generatePseudoPresetMoves(board, moves, sq, KNIGHTBB(SQ120SQ64(sq)), index);
}

static void generatePseudoRookMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int offsets[4] = {-10, -1, 10, 1};
    generateSlidingMoves(board, moves, sq, index, offsets, 4);
}

static void generatePseudoBishopMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    int offsets[4] = {-11, -9, 9, 11};
    generateSlidingMoves(board, moves, sq, index, offsets, 4);
}

static void generatePseudoQueenMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                     int *index) {
    generatePseudoRookMoves(board, moves, sq, index);
    generatePseudoBishopMoves(board, moves, sq, index);
}

static int isAttackedPreset(BOARD_STATE *board, int sq, int enemycolor,
                            ULL bitboard, int genericPiece) {
    if (!ONBOARD(sq)) {
        return FALSE;
    }

    int sq64 = SQ120SQ64(sq);
    ULL bb = (bitboard & board->bitboard[genericPiece] &
              board->bitboard[enemycolor]);

    if (bb == 0) {
        return FALSE;
    }
    return TRUE;
}

static int isAttackedSliding(BOARD_STATE *board, int sq, int *offsets,
                             int sizeoffset, int enemycolor, int RB) {
    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];

        while (ONBOARD(nextSq) &&
               !CHECKBIT(board->bitboard[(bbAny)], SQ120SQ64(nextSq))) {
            nextSq = nextSq + offsets[i];
        }

        if (ONBOARD(nextSq)) {

            ULL bb = (board->bitboard[bbQueen] | board->bitboard[RB]) &
                     board->bitboard[enemycolor];

            if (CHECKBIT(bb, SQ120SQ64(nextSq))) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static int isAttackedPawn(BOARD_STATE *board, int sq, int *offsets,
                          int sizeoffset, int enemycolor) {

    for (int i = 0; i < sizeoffset; i++) {
        int nextSq = sq + offsets[i];

        if (ONBOARD(nextSq) &&
            CHECKBIT(board->bitboard[(enemycolor)], SQ120SQ64(nextSq))) {

            if (CHECKBIT(board->bitboard[(bbPawn)], SQ120SQ64(nextSq))) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

int isAttacked(BOARD_STATE *board, int sq, int enemycolor) {

    int pawn[2] = {-9, 11};
    if (enemycolor == WHITE) {
        pawn[0] = 9;
        pawn[1] = -11;
    }

    int rook[4] = {-10, -1, 10, 1};
    int bishop[4] = {-11, -9, 9, 11};

    return isAttackedPreset(board, sq, enemycolor, KINGBB(SQ120SQ64(sq)),
                            bbKing) ||
           isAttackedPreset(board, sq, enemycolor, KNIGHTBB(SQ120SQ64(sq)),
                            bbKnight) ||
           isAttackedPawn(board, sq, pawn, 2, enemycolor) ||
           isAttackedSliding(board, sq, rook, 4, enemycolor, bbRook) ||
           isAttackedSliding(board, sq, bishop, 4, enemycolor, bbBishop);
}

int isLegalMove(BOARD_STATE *board, MOVE move) {
    int color = board->turn;
    makeMove(board, move);
    int kingsq = board->kings[color];
    int check = isAttacked(board, kingsq, board->turn);
    unmakeMove(board, move);
    return !check;
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;

    // only calc white/black moves on white/black's turn
    ULL bb = board->bitboard[board->turn];

    while (bb != 0) {
        int index64 = bitScanForward(bb);
        int sq = SQ64SQ120(index64);

        int piece = getGenericPieceSq120(sq, board);

        ULL mask = (~1ULL << (index64));
        bb &= mask;

        switch (piece) {
        case bbPawn:
            generatePseudoPawnMoves(board, moves, sq, &index);
            break;
        case bbRook:
            generatePseudoRookMoves(board, moves, sq, &index);
            break;
        case bbBishop:
            generatePseudoBishopMoves(board, moves, sq, &index);
            break;
        case bbKnight:
            generatePseudoKnightMoves(board, moves, sq, &index);
            break;
        case bbQueen:
            generatePseudoQueenMoves(board, moves, sq, &index);
            break;
        case bbKing:
            generatePseudoKingMoves(board, moves, sq, &index);
            break;
        }
    }

    generateCastleMoves(board, moves, &index);

    return index;
}
