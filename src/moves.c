#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void unmakeCastleMove(BOARD_STATE *board, int end) {

    switch (end) {
    case G1:
        clearPiece(board, wR, F1);
        placePiece(board, wR, H1);
        break;
    case C1:
        clearPiece(board, wR, D1);
        placePiece(board, wR, A1);
        break;
    case G8:
        clearPiece(board, bR, F8);
        placePiece(board, bR, H8);
        break;
    case C8:
        clearPiece(board, bR, D8);
        placePiece(board, bR, A8);
        break;
    }
}

static void castleRooks(BOARD_STATE *board, int end) {

    switch (end) {
    case G1:
        board->kings[WHITE] = end;
        clearPiece(board, wR, H1);
        placePiece(board, wR, F1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        break;
    case C1:
        clearPiece(board, wR, A1);
        placePiece(board, wR, D1);
        CLEARBIT(board->castle, WK_CASTLE);
        CLEARBIT(board->castle, WQ_CASTLE);
        board->kings[WHITE] = end;
        break;
    case G8:
        board->kings[BLACK] = end;
        clearPiece(board, bR, H8);
        placePiece(board, bR, F8);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    case C8:
        board->kings[BLACK] = end;
        clearPiece(board, bR, A8);
        placePiece(board, bR, D8);
        CLEARBIT(board->castle, BK_CASTLE);
        CLEARBIT(board->castle, BQ_CASTLE);
        break;
    }
}

// play a move on the board
void makeMove(BOARD_STATE *board, MOVE move) {

    // add info to played moves
    int index = 2 * (board->fullmove - 1) + board->turn;
    board->playedmoves[index].halfmove = board->halfmove;
    board->playedmoves[index].castle = board->castle;
    board->playedmoves[index].enpassant = board->enpassant;

    int piece =
        TOCOLOR(board->turn, getGenericPieceSq120(move.startSquare, board));

    // move rooks for castling moves
    if (move.castle) {
        castleRooks(board, move.endSquare);
    }

    // remove pieces from end square/en passant square
    if (move.enpassant) {

        const int offset = PAWNOFFSET(!board->turn, 0);
        board->enpassant = OFFBOARD;

        clearPiece(board, move.captured, move.endSquare + offset);
    } else if (move.captured != EMPTY) {
        clearPiece(board, move.captured, move.endSquare);
    }

    // update castling permissions
    updateCastling(board, move);

    // move piece to target square
    clearPiece(board, piece, move.startSquare);

    if (move.promotion) {
        placePiece(board, move.promotion, move.endSquare);
    } else {
        placePiece(board, piece, move.endSquare);
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
    board->playedmoves[index].hash = board->hash;

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

    // clear hash
    int index = 2 * (board->fullmove - 1) + board->turn - 1;
    board->playedmoves[index].hash = 0ull;

    // move rooks back
    if (move.castle) {
        unmakeCastleMove(board, move.endSquare);
    }

    // reset castling abilities
    board->castle = board->playedmoves[index].castle;
    board->halfmove = board->playedmoves[index].halfmove;

    int piece =
        TOCOLOR(!board->turn, getGenericPieceSq120(move.endSquare, board));

    // perform en passant
    if (move.enpassant) {
        board->enpassant = move.endSquare;

        const int offset = PAWNOFFSET(board->turn, 0);

        // put back captured piece
        placePiece(board, move.captured, move.endSquare + offset);

        // move pawn back to original square
        clearPiece(board, piece, move.endSquare);
        placePiece(board, piece, move.startSquare);

        if (board->turn == WHITE) {
            --board->fullmove;
        }
        turnZobrist(board);
        board->turn = !(board->turn);
        return;
    }

    // remove piece from end square and replace with captured piece if possible
    clearPiece(board, piece, move.endSquare);
    if (move.captured != EMPTY) {
        placePiece(board, move.captured, move.endSquare);
    }

    // undo promotion
    if (move.promotion == EMPTY) {
        placePiece(board, piece, move.startSquare);
    } else {
        int pawn = TOCOLOR(COLOR(piece), bbPawn);
        placePiece(board, pawn, move.startSquare);
    }

    // update king position
    if (GENERIC(piece) == bbKing) {
        board->kings[!board->turn] = move.startSquare;
    }

    board->enpassant = board->playedmoves[index].enpassant;

    if (board->turn == WHITE) {
        --board->fullmove;
    }

    turnZobrist(board);
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

    moves[*index].enpassant = enpassant;
    moves[*index].twopawnmove = twopawnmove;
    moves[*index].castle = castle;

    moves[*index].check = 0;

    ++(*index);
}

// same as generateSimpleMoves() but continues in offset direction until not
// possible. Used for sliding pieces (bishop,rook,queen)
static void generateSlidingMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                 int *index, const int *offsets,
                                 int offsetssize) {

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
                          int captured, int *index) {

    for (int i = 0; i < 4; ++i) {
        addMove(board, moves, start, end, captured,
                TOCOLOR(board->turn, PROMOTES[i]), FALSE, FALSE, FALSE, index);
    }
}

// add all pseudolegal pawn moves to moves list
static void generatePseudoPawnMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                    int *index) {
    int secondrank = RANK_2 + board->turn * 5;
    int eighthrank = RANK_8 - board->turn * 7;

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
                addPromotions(board, moves, sq, enemysquare, enemypiece, index);
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
    generateSlidingMoves(board, moves, sq, index, ROOKOFFSETS, 4);
}

static void generatePseudoBishopMoves(BOARD_STATE *board, MOVE *moves, int sq,
                                      int *index) {
    generateSlidingMoves(board, moves, sq, index, BISHOPOFFSETS, 4);
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
    const int push = PAWNOFFSET(!board->turn, 0);
    ULL promote;

    switch (board->turn) {
    case WHITE:
        // white pawns
        up = (pawns << 1);
        promote = 0x8080808080808080;
        break;

    case BLACK:
        // black pawns
        up = (pawns >> 1);
        promote = 0x0101010101010101;
        break;
    default:
        return;
    }

    ULL upopen = up & ~(board->bitboard[bbWhite] | board->bitboard[bbBlack]);

    ULL bbone = upopen & ~(promote);
    ULL bbpromote = upopen & promote;

    BITLOOP(bbpromote) {

        int one = bitScanForward(bbpromote);

        addPromotions(board, moves, SQ64SQ120(one + push), SQ64SQ120(one),
                      EMPTY, index);
    }

    BITLOOP(bbone) {

        int one = bitScanForward(bbone);

        addMove(board, moves, SQ64SQ120(one + push), SQ64SQ120(one), EMPTY,
                EMPTY, FALSE, FALSE, FALSE, index);
    }
}

// generate all legal moves and insert them into the moves list
int generateMoves(BOARD_STATE *board, MOVE *moves) {

    int index = 0;

    generatePseudoEnPassantMoves(board, moves, &index);

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

    generateOnePawnMoves(board, moves, &index);

    ULL pawns = board->bitboard[board->turn] & board->bitboard[bbPawn];

    BITLOOP(pawns) {
        int i64 = bitScanForward(pawns);
        generatePseudoPawnMoves(board, moves, SQ64SQ120(i64), &index);
    }

    generateCastleMoves(board, moves, &index);

    return index;
}
