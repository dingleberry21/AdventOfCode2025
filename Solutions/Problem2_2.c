#include <bits/floatn-common.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libs/Bnsparser.h"

/* Problem 2 part 2 of the Advent of Code 2025 challenge. */
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

/* Excluding 2 from the count since we use this function nly to discriminate between
numbers that have a prime amunt f digits and those who dn't, yet 2 is a special number that allows
couples to form, whereas any ther amount of prime digits will not do that.
Also, we stop at 13, since there aren't numbers with that many digits. It's a challenge, not a bomb-proof program, lol. */
bool
is_prime(int n) {
    if ( n == 1 || n == 3 || n == 5 || n == 7 || n == 11 || n == 13) return true;
    return false;
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
            min = min * 10 + ( subtokensbuff[ 0 ]->buff[ i ] - '0' );
        }
        for ( unsigned int i = 0; i < subtokensbuff[1]->length; i++ ) {
            max = max * 10 + ( subtokensbuff[ 1 ]->buff[ i ] - '0' );
        }

        
        // in any number with a non-prime number of digits, we can compose numbers that have as many symmetries as the divisors
        // e.g. between 565656 and 12345678 we will have sets of numbers of 4, 6 and 8 digits.
        // if it's 4 digits: abab or aaaa; 6 digits: abcabc, ababab, aaaaaa; 8 digits: abcd, abababab, aaaaaaaa
        // So it's clear the possibilities depend on the divisors of the number of digits.
        // Ideally: We divide the number of digits by all the possible divisors, and get the size of the module that will be repeated, of length: digits(N)/divisor
        
        
        for ( unsigned int current_digits = min_digits; current_digits < max_digits; current_digits++ ) {
            if ( is_prime( current_digits ) && max_digits > min_digits ) { // if prime we go to the next digit
                current_digits++;
                counter++; // on, suppose, 5 digits, only possible sequence will be `aaaaa`, since there is not other
                // way to fill it without adding a number that forms no repetition
                // e.g. 11112 isn't interesting for our purpose, since 2 "breaks" the repetitions
                continue;
            }

            // finding divisors
            size_t current = min;
            for ( size_t div = 1; div <= current_digits; div++ ) {
                if ( min_digits % div != 0 ) continue;

                // now `div` is a divisor of current_digits
                // on n digits, to get the first `div` (div < n) digits we must divide by 10 exactly n-m times.
                for ( size_t j = 0; j < current_digits-div; j++ ) {
                    current /= 10;
                }

                size_t composed = 0;
                while ( true ) {
                    for (size_t x = 0; x < current_digits; x+=current_digits/div) {
                        composed += current*power(10, x); // ab, on, for example, 10 digits becomes: ababababab
                    }
                    if ( composed >  max) {
                        break;
                    }
                    if ( composed > min ) {
                        counter += composed;
                    } 
                }
            }
        }

        current = current->next;
    }

    printf( "-------- INVALID IDS SUM --------\n" );
    printf( "%zu\n", counter );
    
    return 0;
}