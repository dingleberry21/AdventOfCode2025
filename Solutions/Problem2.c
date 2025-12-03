#include <bits/floatn-common.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../libs/Bnsparser.h"

/* Problem 2 of the Advent of Code 2025 challenge. */
/* Tokeniser used: Bnsparser. Check out Problem 1 for more infos and a link. */

#define DEFAULT_SUBTOKENSBUFF_COUNT 10

token**
subtokeniser( token *T, char *separators_string ) {
    size_t subtokens = DEFAULT_SUBTOKENSBUFF_COUNT;
    token **subtokensbuff = ( token** )xmalloc( sizeof(token *)*subtokens );
    
    size_t last = 0;
    unsigned int pos = 0;
    for ( unsigned int i = 0; i < T->length; i++ ) {
        if ( i == last ) continue;
        
        if ( i == T->length - 1 || validate_separator( T->buff[i], separators_string ) ) {
            token *subT = ( token * )xmalloc( sizeof( token ) );
            subT->buff = &T->buff[ last ];
            subT->length = ( i == T->length-1 ) ? i - last + 1 : i - last;
            subT->next = nullptr;

            subtokensbuff[ pos++ ] = subT;

            last = i+1;
        }
    }

    return subtokensbuff;
}

unsigned int
power( unsigned int base, unsigned int exp ) {
    unsigned int result = 1;
    for ( unsigned int i = 0; i < exp; i++ ) {
        result *= base;
    }
    return result;
}

int main(void) {
    FILE *fp = fopen( "Inputs/Problem2.txt", "r" );
    if ( !fp ) return -1;

    file *F = bnsreadf( fp );
    if ( !F || !F->first ) return -1;

    token **tokensbuff = bnstokenise(F->first, ",");

    token *current = tokensbuff[0];
    size_t counter = 0;
    
    while ( current ) {
        token **subtokensbuff = subtokeniser(current, "-");
        
        unsigned int min_digits = subtokensbuff[ 0 ]->length;
        unsigned int max_digits = subtokensbuff[ 1 ]->length;

        size_t min = 0;
        size_t max = 0;
        for ( unsigned int i = 0; i < subtokensbuff[0]->length; i++ ) {
            min = min * 10 + (subtokensbuff[0]->buff[i] - '0');
        }
        for ( unsigned int i = 0; i < subtokensbuff[1]->length; i++ ) {
            max = max * 10 + (subtokensbuff[1]->buff[i] - '0');
        }

        if ( min_digits % 2 != 0 ) {
            min = power(10, min_digits);
            min_digits++;
        }
        if ( max_digits % 2 != 0 ) {
            max = power(10, max_digits) - 1;
            max_digits--;
        }

        size_t start_left = min;
        for (size_t i = 0; i < min_digits/2; i++) {
            start_left /= 10;
        }

        size_t R = start_left;
        size_t current_digits = min_digits;
        
        while ( true ) {
            size_t max_for_current_digits = power(10, current_digits/2) - 1;
            if (R > max_for_current_digits) {
                current_digits += 2;
                if (current_digits > max_digits) break;
                R = power(10, current_digits/2 - 1);
            }
            
            size_t symmetric = R * power(10, current_digits/2) + R;
            
            if (symmetric > max) {
                break;
            }
            
            if (symmetric >= min) {
                counter += symmetric;
            }
            
            R++;
        }

        current = current->next;
    }

    printf("-------- INVALID IDS SUM --------\n");
    printf("%zu\n", counter);
    
    return 0;
}