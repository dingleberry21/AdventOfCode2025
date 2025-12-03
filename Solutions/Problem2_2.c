#include <bits/floatn-common.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libs/Bnsparser.h"

/* NOT COMPLETE */

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

/* Returns the first `digits_to_extract` most significant figures from N, with N having `total_digits` figures. */
int
get_digits( size_t N, unsigned int total_digits, unsigned int digits_to_extract ) {
    for ( unsigned int i = 0; i < total_digits - digits_to_extract; i++ ) {
        N /= 10;
    }

    return N;
}

/* Replicates a number N of `N_digits` figures in a number of `final_digits` figures, if possible. Returns 0 if unsuccessful.  */
size_t
replicate( unsigned int N, unsigned int N_digits, unsigned int final_digits ) {
    unsigned int final = 0;
    unsigned int exp = final_digits / N_digits;
    for ( unsigned int i = 0; i < final_digits; i += exp ) {
        N  += N*power(10, i);
    }
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
        

        int max_first_digit = get_digits( max, min_digits, 1 );
        int min_first_digit = get_digits( min, max_digits, 1 );

        for ( size_t i = 1; i < min;  )

        current = current->next;
    }

    printf( "-------- INVALID IDS SUM --------\n" );
    printf( "%zu\n", counter );
    
    return 0;
}