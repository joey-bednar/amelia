#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define INPUTLEN 10000
#define POSSTARTLEN 17
#define POSFENLEN 13

static void playUCIMove(BOARD_STATE *board, int start, int end, char promo) {
    int piece;

    switch (promo) {
    case 'q':
        piece = bbQueen;
        break;
    case 'r':
        piece = bbRook;
        break;
    case 'b':
        piece = bbBishop;
        break;
    case 'n':
        piece = bbKnight;
        break;
    default:
        break;
    }

    // printf("playUCIMove %d %d\n",start,end);

    MOVE moves[MAX_LEGAL_MOVES];
    int n_moves = generateMoves(board, moves);

    for (int i = 0; i < n_moves; ++i) {
        if (moves[i].startSquare == start && moves[i].endSquare == end &&
            isLegalMove(board, moves[i])) {
            if (promo == ' ' || promo == '\n') {
                makeMove(board, moves[i]);
                // printf("move no promote\n");
                return;
            } else if (GENERIC(moves[i].promotion) == piece) {
                makeMove(board, moves[i]);
                // printf("move promote type %d\n",moves[i].type);
                return;
            }
        }
    }
}

static void parseMoves(char *string, BOARD_STATE *board) {

    // find and shift to "moves" occurrence in input
    int i = 7;
    char *ret = strstr(string, " moves ");
    if (ret == NULL) {
        return;
    }
    string = ret;

    while (string[i] != '\n' && string[i] != '\0') {
        if (string[i] == ' ') {
            ++i;
        } else {

            int startSq = CHAR2SQ120(string[i], string[i + 1]);
            int endSq = CHAR2SQ120(string[i + 2], string[i + 3]);

            char promo = string[i + 4];

            // printf("parseMoves: %d %d %c\n",startSq,endSq,promo);

            // printBoard(board);
            playUCIMove(board, startSq, endSq, promo);
            // printBoard(board);

            if (promo == ' ') {
                i = i + 4;
            } else if (promo == '\n') {
                break;
            } else {
                i = i + 5;
            }
        }
    }
}

int loadFEN(char *fen, BOARD_STATE *board, int startIndex) {

    clearBoard(board);

    // load all pieces
    int i = startIndex;

    int index = 0;
    while (fen[i] != ' ') {
        if (fen[i] >= '0' && fen[i] <= '8') {
            int val = (int)(fen[i] - '0');
            index = index + val;
        } else if (fen[i] != '/' && fen[i] != ' ') {
            int file = index % 8;
            int rank = 7 - (index / 8);
            int sq = FR2SQ120(file, rank);
            switch (fen[i]) {
            case 'p':
                setPiece120(bP, sq, board);
                break;
            case 'r':
                setPiece120(bR, sq, board);
                break;
            case 'n':
                setPiece120(bN, sq, board);
                break;
            case 'b':
                setPiece120(bB, sq, board);
                break;
            case 'q':
                setPiece120(bQ, sq, board);
                break;
            case 'k':
                setPiece120(bK, sq, board);
                break;

            case 'P':
                setPiece120(wP, sq, board);
                break;
            case 'R':
                setPiece120(wR, sq, board);
                break;
            case 'N':
                setPiece120(wN, sq, board);
                break;
            case 'B':
                setPiece120(wB, sq, board);
                break;
            case 'Q':
                setPiece120(wQ, sq, board);
                break;
            case 'K':
                setPiece120(wK, sq, board);
                break;
            }
            ++index;
        }
        ++i;
    }

    // set turn
    ++i;
    if (fen[i] == 'w') {
        board->turn = WHITE;
    } else if (fen[i] == 'b') {
        board->turn = BLACK;
    }

    // set castling ability
    board->castle = 0;
    i = i + 2;
    while (fen[i] != ' ') {
        switch (fen[i]) {
        case 'K':
            SETBIT(board->castle, WK_CASTLE);
            break;
        case 'Q':
            SETBIT(board->castle, WQ_CASTLE);
            break;
        case 'k':
            SETBIT(board->castle, BK_CASTLE);
            break;
        case 'q':
            SETBIT(board->castle, BQ_CASTLE);
            break;
        }
        ++i;
    }

    // set en passant
    ++i;
    board->enpassant = OFFBOARD;
    int rank = 0;
    int file = 0;
    while (fen[i] != ' ') {

        if (fen[i] >= 'a' && fen[i] <= 'h') {
            file = (int)(fen[i] - 'a');
        } else if (fen[i] >= '1' && fen[i] <= '8') {
            rank = (int)(fen[i] - '1');
            board->enpassant = FR2SQ120(file, rank);
        }
        ++i;
    }

    while (fen[i] == ' ') {
        ++i;
    }

    // set half move counter
    int halfmove = 0;
    while (fen[i] != ' ') {
        if (fen[i] >= '0' && fen[i] <= '9') {
            halfmove = halfmove * 10;
            halfmove = halfmove + (int)(fen[i] - '0');
        }
        ++i;
    }
    board->halfmove = halfmove;

    while (fen[i] == ' ') {
        ++i;
    }

    // set full move counter
    int fullmove = 0;
    while (fen[i] != ' ' && fen[i] != '\n') {
        if (fen[i] >= '0' && fen[i] <= '9') {
            fullmove = fullmove * 10;
            fullmove = fullmove + (int)(fen[i] - '0');
        }
        ++i;
    }
    board->fullmove = fullmove;

    loadZobrist(board);

    return i;
}

// loads wtime/btime/depth from UCI input
static void parseGo(char *string) {

    // find and shift to "moves" occurrence in input
    char *depthStr = strstr(string, " depth ");
    if (depthStr != NULL) {
        int i = 7;
        int val = 0;
        while (depthStr[i] >= '0' && depthStr[i] <= '9') {
            val = val * 10 + (int)(depthStr[i] - '0');
            i++;
        }
        inputDepth = val;
    }

    // find and shift to "wtime" occurrence in input
    char *wtimeStr = strstr(string, " wtime ");
    if (wtimeStr != NULL) {
        int i = 7;
        int val = 0;
        while (wtimeStr[i] >= '0' && wtimeStr[i] <= '9') {
            val = val * 10 + (int)(wtimeStr[i] - '0');
            i++;
        }
        inputTime[WHITE] = val;
    }

    // find and shift to "wtime" occurrence in input
    char *btimeStr = strstr(string, " btime ");
    if (btimeStr != NULL) {
        int i = 7;
        int val = 0;
        while (btimeStr[i] >= '0' && btimeStr[i] <= '9') {
            val = val * 10 + (int)(btimeStr[i] - '0');
            i++;
        }
        inputTime[BLACK] = val;
    }

    // find and shift to "winc" occurrence in input
    char *wincStr = strstr(string, " winc ");
    if (wincStr != NULL) {
        int i = 6;
        int val = 0;
        while (wincStr[i] >= '0' && wincStr[i] <= '9') {
            val = val * 10 + (int)(wincStr[i] - '0');
            i++;
        }
        inputInc[WHITE] = val;
    }

    // find and shift to "binc" occurrence in input
    char *bincStr = strstr(string, " binc ");
    if (bincStr != NULL) {
        int i = 6;
        int val = 0;
        while (bincStr[i] >= '0' && bincStr[i] <= '9') {
            val = val * 10 + (int)(bincStr[i] - '0');
            i++;
        }
        inputInc[BLACK] = val;
    }
}

void startUCI() {
    BOARD_STATE board;
    initBoard(&board);

    while (TRUE) {
        char input[INPUTLEN];
        for (int i = 0; i < INPUTLEN; ++i) {
            input[i] = '\0';
        }
        fgets(input, INPUTLEN, stdin);

        if (strcmp("ucinewgame", input) == 0) {
            printf("readyok\n");
            initBoard(&board);
        } else if (strncmp("uci", input, 3) == 0) {
            printf("id name Annie\n");
            printf("id author Joey Bednar\n");
            printf("uciok\n");
        } else if (strcmp("isready\n", input) == 0) {
            printf("readyok\n");
        } else if (strncmp("position startpos", input, 17) == 0) {
            initBoard(&board);
            parseMoves(input, &board);
        } else if (strncmp("position fen ", input, 13) == 0) {
            initBoard(&board);
            loadFEN(input, &board, 13);
            parseMoves(input, &board);
        } else if (strncmp("go\n", input, 2) == 0) {
            inputDepth = DEFAULTDEPTH;
            inputInc[WHITE] = DEFAULT_INC;
            inputInc[BLACK] = DEFAULT_INC;
            inputTime[WHITE] = DEFAULT_TIME;
            inputTime[BLACK] = DEFAULT_TIME;
            parseGo(input);
            printBestMove(&board);

        } else if (strcmp("stop\n", input) == 0) {
            // printf("stop\n");
        } else if (strcmp("quit\n", input) == 0) {
            break;
        }
        fflush(stdout);
    }
}
