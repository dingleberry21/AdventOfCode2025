#include <stdio.h>
#include "libs/Bnsparser.h"
#include <stdbool.h>

/* For solving this problem I used my custom parsing library (Bnsparser.c) */
/* Find it at: https://github.com/dingleberry21/Bnsparser */

unsigned int power(unsigned int base, unsigned int exp) {
    unsigned int result = 1;
    for (unsigned int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

int main(void) {
    char *filename = "aoc.txt";
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    file *F = bnsreadf(fp);
    bool status = true;
    int pos = 50;
    int psswd = 0;
    
    while (status){
        if (!F->current) break;

        int rot = 0;
        for (unsigned int i = 1; i < F->current->length; i++) {
            rot += (F->current->buff[i] - 48)*power(10, F->current->length-i-1);
        }
        if (F->current->buff[0] == 'R') {
            pos += rot;
        } else {
            pos -= rot;
        }
        
        pos = pos % 100;
        if (pos < 0) {
            pos = 100 + pos;
        }

        if (pos == 0) {
            psswd++;
        }

        status = F->vt->next(F);
    }

    printf("Password: %d\n", psswd);

    return 0;
}