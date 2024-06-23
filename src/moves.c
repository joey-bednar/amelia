#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void unsafeClearPiece(BOARD_STATE *board, int piece, int sq) {
    // move piece to target square
    CLEARBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[bbAny], SQ120SQ64(sq));
    updateZobrist(SQ120SQ64(sq), piece, board);
}

static void unsafePlacePiece(BOARD_STATE *board, int piece, int sq) {
    SETBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[bbAny], SQ120SQ64(sq));
    updateZobrist(SQ120SQ64(sq), piece, board);
}

static void updateCastling(BOARD_STATE *board, MOVE move) {

    int piece = getGenericPieceSq120(move.startSquare, board);

    if (piece == bbRook) {
        if (move.startSquare == H1) {
            CLEARBIT(board->castle, WK_CASTLE);
        } else if (move.startSquare == A1) {
            CLEARBIT(board->castle, WQ_CASTLE);
        } else if (move.startSquare == H8) {
            CLEARBIT(board->castle, BK_CASTLE);
        } else if (move.startSquare == A8) {
            CLEARBIT(board->castle, BQ_CASTLE);
        }
    } else if (piece == bbKing) {
        if (move.startSquare == E1) {
            CLEARBIT(board->castle, WK_CASTLE);
            CLEARBIT(board->castle, WQ_CASTLE);
        } else if (move.endSquare == E8) {
            CLEARBIT(board->castle, BK_CASTLE);
            CLEARBIT(board->castle, BQ_CASTLE);
        }
    }
}

static void unmakeCastleMove(BOARD_STATE *board, int end) {

    switch (end) {
    case G1:
        unsafeClearPiece(board, wR, F1);
        unsafePlacePiece(board, wR, H1);
        break;
    case C1:
        unsafeClearPiece(board, wR, D1);
        unsafePlacePiece(board, wR, A1);
        break;
    case G8:
        unsafeClearPiece(board, bR, F8);
        unsafePlacePiece(board, bR, H8);
        break;
    case C8:
        unsafeClearPiece(board, bR, D8);
        unsafePlacePiece(board, bR, A8);
        break;
    }
}

static void castleRooks(BOARD_STATE *board, int end) {

    switch (end) {
    case G1:
        board->kings[WHITE] = end;
        unsafeClearPiece(board, wR, H1);
        unsafePlacePiece(board, wR, F1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        break;
    case C1:
        unsafeClearPiece(board, wR, A1);
        unsafePlacePiece(board, wR, D1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        board->kings[WHITE] = end;
        break;
    case G8:
        board->kings[BLACK] = end;
        unsafeClearPiece(board, bR, H8);
        unsafePlacePiece(board, bR, F8);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    case C8:
        board->kings[BLACK] = end;
        unsafeClearPiece(board, bR, A8);
        unsafePlacePiece(board, bR, D8);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    }
}

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {
    int piece =
        TOCOLOR(board->turn, getGenericPieceSq120(move.startSquare, board));

    // move rooks for castling moves
    if (move.castle) {
        castleRooks(board, move.endSquare);
    }

    // remove pieces from end square/en passant square
    if (move.enpassant) {
        const int offset[2] = {S, N};
        board->enpassant = OFFBOARD;

        unsafeClearPiece(board, move.captured,
                         move.endSquare + offset[board->turn]);
    } else if (move.captured != EMPTY) {
        for (int i = 0; i <= bbAny; ++i) {
            CLEARBIT(board->bitboard[i], SQ120SQ64(move.endSquare));
        }
        unsafeClearPiece(board, move.captured, move.endSquare);
    }

    // update castling permissions
    updateCastling(board, move);

    // move piece to target square
    unsafeClearPiece(board, piece, move.startSquare);

    if (move.promotion) {
        unsafePlacePiece(board, move.promotion, move.endSquare);
    } else {
        unsafePlacePiece(board, piece, move.endSquare);
    }

    // update king position
    if (CHECKBIT(board->bitboard[bbKing], SQ120SQ64(move.endSquare))) {
        board->kings[board->turn] = move.endSquare;
    }

    // update en passant square
    if (move.twopawnmove) {
        const int offset = PAWNOFFSET(!board->turn, 0);
        board->enpassant = move.endSquare + offset;
    } else {
        board->enpassant = OFFBOARD;
    }

    // update half move
    if (GENERIC(piece) == bbPawn || move.captured != EMPTY) {
        board->halfmove = 0;
    } else {
        ++board->halfmove;
    }

    // add hash to played moves
    board->playedmoves[2 * (board->fullmove - 1) + board->turn] = board->hash;

    // update full move
    if (board->turn == BLACK) {
        ++board->fullmove;
    }

    turnZobrist(board);
    board->turn = !(board->turn);
    return;
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {

    board->playedmoves[2 * (board->fullmove - 1) + board->turn - 1] = 0;

    // move rooks back
    if (move.castle) {
        unmakeCastleMove(board, move.endSquare);
    }

    // reset castling abilities
    board->castle = move.priorcastle;
    board->halfmove = move.priorhalf;

    int piece =
        TOCOLOR(!board->turn, getGenericPieceSq120(move.endSquare, board));

    // perform en passant
    if (move.enpassant) {
        board->enpassant = move.endSquare;

        const int offset = PAWNOFFSET(board->turn, 0);

        // put back captured piece
        unsafePlacePiece(board, move.captured, move.endSquare + offset);

        // move pawn back to original square
        unsafeClearPiece(board, piece, move.endSquare);
        unsafePlacePiece(board, piece, move.startSquare);

        if (board->turn == WHITE) {
            --board->fullmove;
        }
        turnZobrist(board);
        board->turn = !(board->turn);
        return;
    }

    // remove piece from end square and replace with captured piece if possible
    unsafeClearPiece(board, piece, move.endSquare);
    if (move.captured != EMPTY) {
        unsafePlacePiece(board, move.captured, move.endSquare);
    }

    // undo promotion
    if (move.promotion == EMPTY) {
        unsafePlacePiece(board, piece, move.startSquare);
    } else {
        int pawn = TOCOLOR(COLOR(piece), bbPawn);
        unsafePlacePiece(board, pawn, move.startSquare);
    }

    // update king position
    if (GENERIC(piece) == bbKing) {
        board->kings[!board->turn] = move.startSquare;
    }

    board->enpassant = move.priorep;

    if (board->turn == WHITE) {
        --board->fullmove;
    }
    board->turn = !(board->turn);
}

// add a move to the provided moves array
static void addMove(BOARD_STATE *board, MOVE *moves, int start, int end,
                    int captured, int promotion, int enpassant, int twopawnmove,
                    int castle, int *index) {
    // moves[*index].piece = piece;
    moves[*index].startSquare = start;
    moves[*index].endSquare = end;
    moves[*index].captured = captured;
    moves[*index].promotion = promotion;
    moves[*index].priorep = board->enpassant;
    moves[*index].priorcastle = board->castle;
    moves[*index].priorhalf = board->halfmove;

    moves[*index].enpassant = enpassant;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castle = castle;

    moves[*index].check = 0;

    ++(*index);

    // flags:
    // castle
    // capture
    // enpassant
    // twopawnmove
    // promotion
    // check

    // types
    // simple
    // capture simple
    // en passant capture
    // two pawn move
    // simple promotion
    // capture promotion
    // castle 4 types
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, int *offsets, int offsetssize) {

    for (int i = 0; i < offsetssize; ++i) {
        int nextSq = sq + offsets[i];

        // if square is empty or can capture enemy piece, it is a pseudolegal
        // move

        while (isEmptySquare(nextSq, board)) {
            addMove(board, moves, sq, nextSq, EMPTY, EMPTY, FALSE, FALSE, FALSE,
                    index);

            nextSq = nextSq + offsets[i];
        }

        if (ONBOARD(nextSq) &&
            CHECKBIT(board->bitboard[(!board->turn)], SQ120SQ64(nextSq))) {
            int squareContains =
                TOCOLOR(!board->turn, getGenericPieceSq120(nextSq, board));
            addMove(board, moves, sq, nextSq, squareContains, EMPTY, FALSE,
                    FALSE, FALSE, index);
        }
    }
}

// add all legal castling moves to the moves array
static void generateCastleMoves(BOARD_STATE *board, MOVE *moves, int *index) {

    int rank = RANK_1;
    int queenside = WQ_CASTLE;
    int kingside = WK_CASTLE;
    if (board->turn == BLACK) {
        rank = RANK_8;
        queenside = BQ_CASTLE;
        kingside = BK_CASTLE;
    }

    int color = board->turn;

    if (isAttacked(board, board->kings[color], !color)) {
        return;
    }

    ULL bb = board->bitboard[bbRook] & board->bitboard[color];

    if (board->kings[color] == FR2SQ120(FILE_E, rank)) {
        if (CHECKBIT(board->castle, kingside) &&
            CHECKBIT(bb, FR2SQ64(FILE_H, rank))) {

            int throughcheck =
                isAttacked(board, FR2SQ120(FILE_F, rank), !color) ||
                isAttacked(board, FR2SQ120(FILE_G, rank), !color);

            int blocked =
                CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                         FR2SQ64(FILE_F, rank)) ||
                CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                         FR2SQ64(FILE_G, rank));

            if (!throughcheck && !blocked) {
                addMove(board, moves, FR2SQ120(FILE_E, rank),
                        FR2SQ120(FILE_G, rank), EMPTY, EMPTY, FALSE, FALSE,
                        TRUE, index);
            }
        }

        if (CHECKBIT(board->castle, queenside) &&
            CHECKBIT(bb, FR2SQ64(FILE_A, rank))) {

            int throughcheck =
                isAttacked(board, FR2SQ120(FILE_C, rank), !color) ||
                isAttacked(board, FR2SQ120(FILE_D, rank), !color);

            int blocked =
                CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                         FR2SQ64(FILE_B, rank)) ||
                CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                         FR2SQ64(FILE_C, rank)) ||
                CHECKBIT(board->bitboard[bbWhite] | board->bitboard[bbBlack],
                         FR2SQ64(FILE_D, rank));

            if (!throughcheck && !blocked) {
                addMove(board, moves, FR2SQ120(FILE_E, rank),
                        FR2SQ120(FILE_C, rank), EMPTY, EMPTY, FALSE, FALSE,
                        TRUE, index);
            }
        }
    }
}

// used by generatePseudoPawnMoves. add all four possible promotion moves to
// moves list.
static void addPromotions(BOARD_STATE *board, MOVE *moves, int start, int end,
                          int captured, int color, int *index) {
    int promoteTo[4] = {bbQueen, bbKnight, bbRook, bbBishop};

    for (int i = 0; i < 4; ++i) {
        addMove(board, moves, start, end, captured,
                TOCOLOR(board->turn, promoteTo[i]), FALSE, FALSE, FALSE, index);
    }
}

// add all pseudolegal pawn moves to moves list
static void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int color = board->turn;

    int secondrank = RANK_2 + board->turn * 5;
    int eighthrank = RANK_8 - board->turn * 7;
    int enemypawn = TOCOLOR(!board->turn, bbPawn);

    int one = sq + PAWNOFFSET(board->turn, 0);
    int two = sq + PAWNOFFSET(board->turn, 1);

    // captures excluding en passant
    for (int i = 2; i <= 3; ++i) {
        int enemysquare = sq + PAWNOFFSET(board->turn, i);

        if (!ONBOARD(enemysquare)) {
            continue;
        }

        if (CHECKBIT(board->bitboard[(!board->turn)], SQ120SQ64(enemysquare))) {
            int enemypiece =
                TOCOLOR(!board->turn, getGenericPieceSq120(enemysquare, board));

            if (SQ120R(enemysquare) == eighthrank) {
                addPromotions(board, moves, sq, enemysquare, enemypiece, color,
                              index);
            } else {
                addMove(board, moves, sq, enemysquare, enemypiece, EMPTY, FALSE,
                        FALSE, FALSE, index);
            }
        }
    }

    // up two
    if (SQ120R(sq) == secondrank && isEmptySquare(one, board) &&
        isEmptySquare(two, board)) {
        addMove(board, moves, sq, two, EMPTY, EMPTY, FALSE, TRUE, FALSE, index);
    }
}

// use pregenerated attack bitboards to generate all pseudolegal moves
// and add to array.
static void generatePseudoPresetMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      ULL bitboard, int *index) {
    ULL bb = ~board->bitboard[board->turn] & bitboard;

    BITLOOP(bb) {
        int nextSq64 = bitScanForward(bb);
        int nextSq120 = SQ64SQ120(nextSq64);

        int squareContains = getPieceSq120(nextSq120, board);
        addMove(board, moves, sq, nextSq120, squareContains, EMPTY, FALSE,
                FALSE, FALSE, index);
    }
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

static void generatePseudoEnPassantMoves(BOARD_STATE *board, MOVE *moves,
                                         int *index) {

    // en passant not possible if square is offboard
    if (!ONBOARD(board->enpassant)) {
        return;
    }

    ULL bb = board->bitboard[board->turn] & board->bitboard[bbPawn];

    for (int i = 2; i <= 3; ++i) {

        int sq = board->enpassant + PAWNOFFSET(!board->turn, i);

        // check all squares that could capture en passant for
        // same color pawns
        if (ONBOARD(sq) && CHECKBIT(bb, SQ120SQ64(sq))) {

            addMove(board, moves, sq, board->enpassant,
                    TOCOLOR(!board->turn, bbPawn), EMPTY, TRUE, FALSE, FALSE,
                    index);
        }
    }
}

void generateOnePawnMoves(BOARD_STATE *board, MOVE *moves, int *index) {

    ULL pawns = board->bitboard[board->turn] & board->bitboard[bbPawn];
    ULL up;
    int push;
    ULL promote;

    switch (board->turn) {
    case WHITE:
        // white pawns
        up = (pawns << 1);
        push = -1;
        promote = 0x8080808080808080;
        break;

    case BLACK:
        // black pawns
        up = (pawns >> 1);
        push = 1;
        promote = 0x0101010101010101;
        break;
    }

    ULL upopen = up & ~(board->bitboard[bbWhite] | board->bitboard[bbBlack]);

    ULL bbone = upopen & ~(promote);
    ULL bbpromote = upopen & promote;

    BITLOOP(bbpromote) {

        int one = bitScanForward(bbpromote);

        int rank = SQ120R(SQ64SQ120(one));

        addPromotions(board, moves, SQ64SQ120(one + push), SQ64SQ120(one),
                      EMPTY, board->turn, index);
    }

    BITLOOP(bbone) {

        int one = bitScanForward(bbone);

        int rank = SQ120R(SQ64SQ120(one));

        addMove(board, moves, SQ64SQ120(one + push), SQ64SQ120(one), EMPTY,
                EMPTY, FALSE, FALSE, FALSE, index);
    }
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;

    generateOnePawnMoves(board, moves, &index);

    ULL pawns = board->bitboard[board->turn] & board->bitboard[bbPawn];

    BITLOOP(pawns) {
        int i64 = bitScanForward(pawns);
        generatePseudoPawnMoves(board, moves, SQ64SQ120(i64), &index);
    }

    ULL rooks = board->bitboard[board->turn] & board->bitboard[bbRook];
    BITLOOP(rooks) {
        int i64 = bitScanForward(rooks);
        generatePseudoRookMoves(board, moves, SQ64SQ120(i64), &index);
    }

    ULL bishops = board->bitboard[board->turn] & board->bitboard[bbBishop];
    BITLOOP(bishops) {
        int i64 = bitScanForward(bishops);
        generatePseudoBishopMoves(board, moves, SQ64SQ120(i64), &index);
    }

    ULL knights = board->bitboard[board->turn] & board->bitboard[bbKnight];
    BITLOOP(knights) {
        int i64 = bitScanForward(knights);

        generatePseudoPresetMoves(board, moves, SQ64SQ120(i64), KNIGHTBB(i64),
                                  &index);
    }

    ULL kings = board->bitboard[board->turn] & board->bitboard[bbKing];
    BITLOOP(kings) {
        int i64 = bitScanForward(kings);
        generatePseudoPresetMoves(board, moves, SQ64SQ120(i64), KINGBB(i64),
                                  &index);
    }

    ULL queens = board->bitboard[board->turn] & board->bitboard[bbQueen];
    BITLOOP(queens) {
        int i64 = bitScanForward(queens);

        generatePseudoRookMoves(board, moves, SQ64SQ120(i64), &index);
        generatePseudoBishopMoves(board, moves, SQ64SQ120(i64), &index);
    }

    generatePseudoEnPassantMoves(board, moves, &index);
    generateCastleMoves(board, moves, &index);

    return index;
}
