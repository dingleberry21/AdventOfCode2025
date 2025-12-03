#include <stdio.h>
#include "../libs/Bnsparser.h"

/* NOT WORKING */

/* Day 3 of Avent of Code 2025 challenge. */

int main(void) {
    FILE *fp = fopen( "Inputs/Problem2.txt", "r" );
    if ( !fp ) return -1;

    file *F = bnsreadf( fp );
    if ( !F->first ) { // F can't be returns as nullptr anyways
        return -1;
    }

    line *current = F->first;
    int result = 0;

    while ( current ) {
        int max_joltage = 0;

        for ( unsigned int i = 0; i < current->length; i++ ) {            
            int d1 = current->buff[i] - '0';

            for ( unsigned int j = i+1; j < current->length; j++ ) {
                int d2 = current->buff[j] - '0';

                int current_joltage = 10 * d1 + d2;
                
                if ( current_joltage > max_joltage ) {
                    printf("%i\n", current_joltage);
                    result += current_joltage;
                    max_joltage = current_joltage;
                }
            }
        }

        current = current->next;
    }

    printf( "Result:%i\n", result );

    return 0;
}