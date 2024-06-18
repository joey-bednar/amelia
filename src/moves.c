#include "defs.h"
#include <assert.h>
#include <stdio.h>

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
    if (end == G1) {
        CLEARBIT(board->bitboard[bbRook], SQ120SQ64(F1));
        CLEARBIT(board->bitboard[bbWhite], SQ120SQ64(F1));
        CLEARBIT(board->bitboard[bbAny], SQ120SQ64(F1));
        SETBIT(board->bitboard[bbRook], SQ120SQ64(H1));
        SETBIT(board->bitboard[bbWhite], SQ120SQ64(H1));
        SETBIT(board->bitboard[bbAny], SQ120SQ64(H1));
    } else if (end == C1) {
        CLEARBIT(board->bitboard[bbRook], SQ120SQ64(D1));
        CLEARBIT(board->bitboard[bbWhite], SQ120SQ64(D1));
        CLEARBIT(board->bitboard[bbAny], SQ120SQ64(D1));
        SETBIT(board->bitboard[bbRook], SQ120SQ64(A1));
        SETBIT(board->bitboard[bbWhite], SQ120SQ64(A1));
        SETBIT(board->bitboard[bbAny], SQ120SQ64(A1));
    } else if (end == G8) {
        CLEARBIT(board->bitboard[bbRook], SQ120SQ64(F8));
        CLEARBIT(board->bitboard[bbBlack], SQ120SQ64(F8));
        CLEARBIT(board->bitboard[bbAny], SQ120SQ64(F8));
        SETBIT(board->bitboard[bbRook], SQ120SQ64(H8));
        SETBIT(board->bitboard[bbBlack], SQ120SQ64(H8));
        SETBIT(board->bitboard[bbAny], SQ120SQ64(H8));
    } else if (end == C8) {
        CLEARBIT(board->bitboard[bbRook], SQ120SQ64(D8));
        CLEARBIT(board->bitboard[bbBlack], SQ120SQ64(D8));
        CLEARBIT(board->bitboard[bbAny], SQ120SQ64(D8));
        SETBIT(board->bitboard[bbRook], SQ120SQ64(A8));
        SETBIT(board->bitboard[bbBlack], SQ120SQ64(A8));
        SETBIT(board->bitboard[bbAny], SQ120SQ64(A8));
    }
}

static void unsafeClearPiece(BOARD_STATE *board, int piece, int sq) {
    // move piece to target square
    CLEARBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[bbAny], SQ120SQ64(sq));
}

static void unsafePlacePiece(BOARD_STATE *board, int piece, int sq) {
    SETBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[COLOR(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[bbAny], SQ120SQ64(sq));
}

static void castleRooks(BOARD_STATE *board, int end) {
    if (end == G1) {
        board->kings[WHITE] = end;

        unsafeClearPiece(board, wR, H1);
        unsafePlacePiece(board, wR, F1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (end == C1) {

        board->kings[WHITE] = end;

        unsafeClearPiece(board, wR, A1);
        unsafePlacePiece(board, wR, D1);

        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (end == C8) {
        board->kings[BLACK] = end;

        unsafeClearPiece(board, bR, A8);
        unsafePlacePiece(board, bR, D8);

        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
    } else if (end == G8) {
        board->kings[BLACK] = end;

        unsafeClearPiece(board, bR, H8);
        unsafePlacePiece(board, bR, F8);

        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
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

        for (int i = 0; i <= bbAny; i++) {
            CLEARBIT(board->bitboard[i],
                     SQ120SQ64(move.endSquare + offset[board->turn]));
        }
    } else if (move.captured != EMPTY) {
        for (int i = 0; i <= bbAny; i++) {
            CLEARBIT(board->bitboard[i], SQ120SQ64(move.endSquare));
        }
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
        const int offset[2] = {S, N};
        board->enpassant = move.endSquare + offset[board->turn];
    } else {
        board->enpassant = OFFBOARD;
    }

    board->turn = !(board->turn);
    return;
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {

    // move rooks back
    if (move.castle) {
        unmakeCastleMove(board, move.endSquare);
    }

    // reset castling abilities
    board->castle = move.priorcastle;

    int piece =
        TOCOLOR(!board->turn, getGenericPieceSq120(move.endSquare, board));

    // perform en passant
    if (move.enpassant) {
        board->enpassant = move.endSquare;

        int offset = S;
        if (board->turn == WHITE) {
            offset = N;
        }

        // int offset = -2*board->turn + 1;

        // put back captured piece
        unsafePlacePiece(board, move.captured, move.endSquare + offset);

        // move pawn back to original square
        unsafeClearPiece(board, piece, move.endSquare);
        unsafePlacePiece(board, piece, move.startSquare);
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
        // setPiece120(piece, move.startSquare, board);
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

    moves[*index].enpassant = enpassant;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castle = castle;

    (*index)++;

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

    for (int i = 0; i < offsetssize; i++) {
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
    int promoteTo[4] = {wQ, wN, wR, wB};

    if (color == BLACK) {
        promoteTo[0] = bQ;
        promoteTo[1] = bN;
        promoteTo[2] = bR;
        promoteTo[3] = bB;
    }
    for (int i = 0; i < 4; i++) {
        addMove(board, moves, start, end, captured, promoteTo[i], FALSE, FALSE,
                FALSE, index);
    }
}

// add all pseudolegal pawn moves to moves list
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
            addMove(board, moves, sq, one, EMPTY, EMPTY, FALSE, FALSE, FALSE,
                    index);
        }
    }

    // captures and en passant
    for (int i = 2; i <= 3; i++) {
        int enemysquare = sq + offset[i];

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

    if (!ONBOARD(board->enpassant)) {
        return;
    }

    int sq;
    if (board->turn == WHITE) {
        sq = board->enpassant + SW;
        if (getPieceSq120(sq, board) == wP) {

            addMove(board, moves, sq, board->enpassant, bP, EMPTY, TRUE, FALSE,
                    FALSE, index);
        }

        sq = board->enpassant + SE;
        if (getPieceSq120(sq, board) == wP) {
            addMove(board, moves, sq, board->enpassant, bP, EMPTY, TRUE, FALSE,
                    FALSE, index);
        }
    }

    if (board->turn == BLACK) {
        sq = board->enpassant + NW;
        if (getPieceSq120(sq, board) == bP) {
            addMove(board, moves, sq, board->enpassant, wP, EMPTY, TRUE, FALSE,
                    FALSE, index);
        }

        sq = board->enpassant + NE;
        if (getPieceSq120(sq, board) == bP) {
            addMove(board, moves, sq, board->enpassant, wP, EMPTY, TRUE, FALSE,
                    FALSE, index);
        }
    }
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;

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
