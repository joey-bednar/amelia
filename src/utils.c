#include "defs.h"
#include <assert.h>
#include <stdio.h>

void printBits(unsigned long num) {
    int bits = sizeof(num) * 8;
    unsigned long mask = 1UL << (bits - 1);

    for (int i = 0; i < bits; i++) {
        putchar((num & mask) ? '1' : '0');
        num <<= 1;
        if ((i + 1) % 8 == 0) {
            putchar('\n');
        }
    }
}

int countBits(unsigned long b) {
    int r;
    for (r = 0; b; r++, b &= b - 1)
        ;
    return r;
}
