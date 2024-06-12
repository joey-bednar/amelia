#include "defs.h"
#include <stdio.h>
#include <string.h>

#define INPUTLEN 256

void startUCI() {
    BOARD_STATE board;

    // while(TRUE) {
    //     char input[INPUTLEN];
    //     fgets(input,INPUTLEN,stdin);
    //
    //     printf("%s\n",input);
    //     printf("%d\n",strcmp(input,"uci\n"));
    // }

    while (TRUE) {
        char input[INPUTLEN];
        fgets(input, INPUTLEN, stdin);
        if (strcmp("uci\n", input) == 0) {
            printf("uciok\n");
        } else if (strcmp("isready\n", input) == 0) {
            printf("readyok\n");
        } else if (strcmp("ucinewgame\n", input) == 0) {
            // printf("readyok\n");
        } else if (strcmp("position startpos\n", input) == 0) {
            initBoard(&board);
            printf("Starting position\n");
        } else if (strncmp("position startpos moves \n", input, 23) == 0) {
            initBoard(&board);
            printf("new position\n");
        } else if (strncmp("go\n", input, 2) == 0) {
            printf("go\n");
        } else if (strcmp("stop\n", input) == 0) {
            printf("stop\n");
        } else if (strcmp("quit\n", input) == 0) {
            break;
        }
        fflush(stdin);
    }
}
