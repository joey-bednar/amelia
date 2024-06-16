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
    CLEARBIT(board->bitboard[board->turn], SQ120SQ64(sq));
    CLEARBIT(board->bitboard[bbAny], SQ120SQ64(sq));
}

static void unsafePlacePiece(BOARD_STATE *board, int piece, int sq) {
    SETBIT(board->bitboard[GENERIC(piece)], SQ120SQ64(sq));
    SETBIT(board->bitboard[board->turn], SQ120SQ64(sq));
    SETBIT(board->bitboard[bbAny], SQ120SQ64(sq));
}

static void makeCastleMove(BOARD_STATE *board, int end) {
    if (end == G1) {
        unsafeClearPiece(board, wK, E1);
        unsafePlacePiece(board, wK, end);
        board->kings[WHITE] = end;

        unsafeClearPiece(board, wR, H1);
        unsafePlacePiece(board, wR, F1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (end == C1) {

        unsafeClearPiece(board, wK, E1);
        unsafePlacePiece(board, wK, end);
        board->kings[WHITE] = end;

        unsafeClearPiece(board, wR, A1);
        unsafePlacePiece(board, wR, D1);

        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
    } else if (end == C8) {
        unsafeClearPiece(board, bK, E8);
        unsafePlacePiece(board, bK, end);
        board->kings[BLACK] = end;

        unsafeClearPiece(board, bR, A8);
        unsafePlacePiece(board, bR, D8);

        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
    } else if (end == G8) {
        unsafeClearPiece(board, bK, E8);
        unsafePlacePiece(board, bK, end);
        board->kings[BLACK] = end;

        unsafeClearPiece(board, bR, H8);
        unsafePlacePiece(board, bR, F8);

        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
    }
}

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {

    if (move.captured != EMPTY && move.promotion == EMPTY && !move.enpassant) {
        int piece = getPieceSq120(move.startSquare, board);

        // move piece to target square
        setPiece120(EMPTY, move.startSquare, board);
        setPiece120(piece, move.endSquare, board);

        // update castling permissions if moving a king/rook
        updateCastling(board, move);

        board->enpassant = OFFBOARD;

        board->turn = !(board->turn);
        return;

    } else if (move.twopawnmove) {

        int piece = TOCOLOR(board->turn, bbPawn);
        // int piece = getPieceSq120(move.startSquare, board);

        // move piece to target square
        unsafeClearPiece(board, piece, move.startSquare);
        unsafePlacePiece(board, piece, move.endSquare);

        // set new en passant square after two square pawn move.
        // delete en passant square if last move wasn't a two square pawn move
        const int offset[2] = {S, N};
        board->enpassant = move.endSquare + offset[board->turn];

        board->turn = !(board->turn);
        return;
    }

    // remove kingside/queenside castling ability
    // after castling. place rook in new position.
    else if (move.castle) {
        makeCastleMove(board, move.endSquare);

        board->enpassant = OFFBOARD;
        board->turn = !(board->turn);
        return;
    }

    else if (move.enpassant) {
        int piece = TOCOLOR(board->turn, bbPawn);

        // move piece to target square
        unsafeClearPiece(board, piece, move.startSquare);
        unsafePlacePiece(board, piece, move.endSquare);

        board->enpassant = OFFBOARD;

        // delete en passant square after performing en passant
        // remove en passanted piece
        const int offset[2] = {S, N};
        board->enpassant = OFFBOARD;
        setPiece120(EMPTY, move.endSquare + offset[board->turn], board);
        board->turn = !(board->turn);
        return;
    } else if (move.promotion != EMPTY) {

        int piece =
            TOCOLOR(board->turn, getGenericPieceSq120(move.startSquare, board));

        // move piece to target square
        setPiece120(EMPTY, move.startSquare, board);
        setPiece120(piece, move.endSquare, board);

        board->enpassant = OFFBOARD;

        setPiece120(move.promotion, move.endSquare, board);

        board->turn = !(board->turn);
        return;
    } else {
        int piece =
            TOCOLOR(board->turn, getGenericPieceSq120(move.startSquare, board));

        updateCastling(board, move);

        // move piece to target square
        unsafeClearPiece(board, piece, move.startSquare);
        unsafePlacePiece(board, piece, move.endSquare);

        if (CHECKBIT(board->bitboard[bbKing], SQ120SQ64(move.endSquare))) {
            board->kings[board->turn] = move.endSquare;
        }

        board->enpassant = OFFBOARD;

        board->turn = !(board->turn);
        return;
    }
}

// undo a move on the board
void unmakeMove(BOARD_STATE *board, MOVE move) {

    if (move.castle) {
        unmakeCastleMove(board, move.endSquare);
    }

    // reset castling abilities
    board->castle = move.priorcastle;

    int offset = S;
    if (board->turn == WHITE) {
        offset = N;
    }

    int piece = getPieceSq120(move.endSquare, board);

    // perform en passant
    if (move.enpassant) {
        board->enpassant = move.endSquare;
        setPiece120(move.captured, move.endSquare + offset, board);
        // unsafeClearPiece(board, piece, move.endSquare);
        setPiece120(EMPTY, move.endSquare, board);
        setPiece120(piece, move.startSquare, board);
        // unsafePlacePiece(board, piece, move.startSquare);
        board->turn = !(board->turn);
        return;
    }

    // put captured piece back
    setPiece120(move.captured, move.endSquare, board);

    // undo promotion
    if (move.promotion == EMPTY) {
        setPiece120(piece, move.startSquare, board);
    } else {
        int pawn = wP;
        if (COLOR(piece) == BLACK) {
            pawn = bP;
        }
        setPiece120(pawn, move.startSquare, board);
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
            int squareContains = getPieceSq120(nextSq, board);
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
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_F, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_G, rank));

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
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_B, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_C, rank)) ||
                CHECKBIT(board->bitboard[bbAny], FR2SQ64(FILE_D, rank));

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
            int enemypiece = getPieceSq120(enemysquare, board);

            if (SQ120R(enemysquare) == eighthrank) {
                addPromotions(board, moves, sq, enemysquare, enemypiece, color,
                              index);
            } else {
                addMove(board, moves, sq, enemysquare, enemypiece, EMPTY, FALSE,
                        FALSE, FALSE, index);
            }
        } else if (epMap[enemysquare] && enemysquare == board->enpassant) {

            addMove(board, moves, sq, enemysquare, enemypawn, EMPTY, TRUE,
                    FALSE, FALSE, index);
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

    while (bb != 0) {
        int nextSq64 = bitScanForward(bb);
        int nextSq120 = SQ64SQ120(nextSq64);

        int squareContains = getPieceSq120(nextSq120, board);
        int type = MOVE_CAPTURE;
        if (squareContains == EMPTY) {
            type = MOVE_QUIET;
        }
        addMove(board, moves, sq, nextSq120, squareContains, EMPTY, FALSE,
                FALSE, FALSE, index);

        ULL mask = (~1ULL << (nextSq64));
        bb &= mask;
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
            generatePseudoPresetMoves(board, moves, sq, KNIGHTBB(SQ120SQ64(sq)),
                                      &index);
            break;
        case bbQueen:
            generatePseudoRookMoves(board, moves, sq, &index);
            generatePseudoBishopMoves(board, moves, sq, &index);
            break;
        case bbKing:
            generatePseudoPresetMoves(board, moves, sq, KINGBB(SQ120SQ64(sq)),
                                      &index);
            break;
        }
    }

    generateCastleMoves(board, moves, &index);

    return index;
}
