#include <bits/floatn-common.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../libs/Bnsparser.h"

/* Problem 2 of the Advent of Code 2025 challenge. */
/* My original solution hasn't worked. Asked Claude to fix it. I am ABSOLUTE ASHAMED
for the way I've treated the challenge. I was close to the answer but still couldn't manage
to finish it because of some more or less serious bugs. After a day of debugging I realised
I was not able to fix it, it was too chaotic. Before deletion I wanted to check if an LLM
was able to fix this, given that also other humans couldn't help with the mess I made.
Claude Sonnet did the job. Gemini, on the other side, is absolutely stupid. */

#define DEFAULT_SUBTOKENSBUFF_COUNT 10

/* Takes one token as a parameter and eventually returns multiple sub-tokens.
`separators_string` MUST be NULL-TERMINATED (either manually add 0 to the array
or simply pass a string. No further sanity controls are made).
Since I mean to change the final implementation, this one will be simplified,
so that we won't care about assigning correct the `.next` field in the token struct,
since in any case for the scope of this challenge we always need to access to elements:
`subtokensbuff[0]` and `subtokensbuff[1]`. */
token**
subtokeniser( token *T, char *separators_string ) {
    size_t subtokens = DEFAULT_SUBTOKENSBUFF_COUNT;
    token **subtokensbuff = ( token** )xmalloc( sizeof(token *)*subtokens );
    
    size_t last = 0;
    unsigned int pos = 0; // used to place correctly in the subtokensbuff
    for ( unsigned int i = 0; i < T->length; i++ ) {
        if ( i == last ) continue;
        
        // capture the sub-token
        if ( i == T->length - 1 || validate_separator( T->buff[i], separators_string ) ) {
            token *subT = ( token * )xmalloc( sizeof( token ) );
            subT->buff = &T->buff[ last ];
            subT->length = ( i == T->length-1 ) ? i - last + 1 : i - last;
            subT->next = nullptr;

            subtokensbuff[ pos++ ] = subT; // positioning in the buffer

            last = i+1; // so we don't catch the same char twice
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

    // all the text is on 1 unique line, the first one.
    token **tokensbuff = bnstokenise(F->first, ","); // comma-separated pairs

    token *current = tokensbuff[0]; // starting from first token
    size_t counter = 0; // final result
    
    while ( current ) {
        token **subtokensbuff = subtokeniser(current, "-"); //always gives us 2 str tokens: min and max
        
        // 1. get the length of the number strings
        unsigned int min_digits = subtokensbuff[ 0 ]->length;
        unsigned int max_digits = subtokensbuff[ 1 ]->length;

        // 2. convert them to numbers - FIXED
        size_t min = 0;
        size_t max = 0;
        for ( unsigned int i = 0; i < subtokensbuff[0]->length; i++ ) {
            min = min * 10 + (subtokensbuff[0]->buff[i] - '0');
        }
        for ( unsigned int i = 0; i < subtokensbuff[1]->length; i++ ) {
            max = max * 10 + (subtokensbuff[1]->buff[i] - '0');
        }

        // 3. we need to start and end where we can operate: a number with an even amount of digits
        if ( min_digits % 2 != 0 ) {
            min = power(10, min_digits); // upgrading to biggest number with an even amount of digits in the range
            min_digits++;
        }
        if ( max_digits % 2 != 0 ) {
            max = power(10, max_digits) - 1; // downgrading
            max_digits--;
        }

        // 4. Calculate starting left half
        size_t start = min;
        for (size_t i = 0; i < min_digits/2; i++) {
            start /= 10;
        }

        // 5. Main loop - iterate through symmetric numbers
        size_t R = start;
        size_t current_digits = min_digits;
        
        printf( "Range: %zu - %zu (digits: %u - %u); Start half: %zu\n", min, max, min_digits, max_digits, start );
        
        while ( true ) {
            // Check if we need to move to next digit length
            size_t max_for_current_digits = power(10, current_digits/2) - 1;
            if (R > max_for_current_digits) {
                current_digits += 2;
                if (current_digits > max_digits) break;
                R = power(10, current_digits/2 - 1);
            }
            
            // Build symmetric number: R concatenated with itself
            size_t symmetric = R * power(10, current_digits/2) + R;
            
            // If we've exceeded the max, we're done with this range
            if (symmetric > max) {
                break;
            }
            
            // If symmetric number is in valid range, add it to counter
            if (symmetric >= min) {
                // printf("Hit: %zu\n", symmetric);
                counter += symmetric;
            }
            
            R++;
        }

        current = current->next;
    }

    printf("-------- INVALID IDS SUM --------\n%zu\n-------- END --------\n", counter);
    return 0;
}