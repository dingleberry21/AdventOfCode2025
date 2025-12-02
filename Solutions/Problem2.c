#include <bits/floatn-common.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../libs/Bnsparser.h"

/* NOTE: NOT functional. Work in progress. */

/* Problem 2 of the Advent of Code 2025 challenge. */
/* As of today (02/12/2025) my Bnsparser doesn't have a token sub-parser, so I can't
get sub-strings from tokens the same way I do with lines when I tokenise them.
I'm building here a custom function that'll do the job.

For infos on my tokeniser, see Problem1.c, where I've included the link to the latest page. */

#define DEFAULT_SUBTOKENSBUFF_COUNT 10

/* Takes one token as a parameter and eventually returns multiple sub-tokens.
`separators_string` MUST be NULL-TERMINATED (either manually add 0 to the array
or simply pass a string. No further sanity controls are made).
Since I mean to change the final implementation, this one will be simplified,
so that we won't care about assigning correct the `.next` field in the token struct,
since in any case for the scope of this challenge we always need to access to elements:
`subtokensbuff[0]` and `subtokensbuff[1]`. */
token** subtokeniser(token *T, char *separators_string) {
    size_t subtokens = DEFAULT_SUBTOKENSBUFF_COUNT;
    token **subtokensbuff = (token**)xmalloc(sizeof(token *)*subtokens);
    
    size_t last = 0;
    unsigned int pos = 0; // used to place correctly in the subtokensbuff
    for (unsigned int i = 0; i < T->length; i++) {
        if (i == last) continue;
        
        // capture the sub-token
        if (i == T->length - 1 || validate_separator(T->buff[i], separators_string)) {
            // TODO: in the repo of the bnsparser remember to check for empty subtokens, so that they're not considered

            token *subT = (token *)xmalloc(sizeof(token));
            subT->buff = &T->buff[last];
            subT->length = (i == T->length-1) ? i - last + 1 : i - last;
            subT->next = nullptr; // TODO - in the Bnsparser repo

            subtokensbuff[pos++] = subT; // positioning in the buffer

            last = i+1; // so we don't catch the same char twice
        }
    }

    return subtokensbuff;
}

unsigned int power(unsigned int base, unsigned int exp) {
    unsigned int result = 1;
    for (unsigned int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

int main(void) {
    FILE *fp = fopen("Inputs/Problem2.txt", "r");
    if (!fp) return -1;

    file *F = bnsreadf(fp);
    if (!F || !F->first) return -1;

    // all the text is on 1 unique line, the first one.
    token **tokensbuff = bnstokenise(F->first, ","); // comma-separated pairs

    token *current = tokensbuff[0]; // starting from first token
    size_t counter = 0; // final result
    size_t min = 0; // INITIALLY stores the LOWER boundary of the range 
    size_t max = 0; // INITIALLY stores the UPPER boundary of the range
    unsigned int min_digits = 0; // digits of the lower boundary
    unsigned int max_digits = 0; // digits of the upper boudary
    while (current) {
        token **subtokensbuff = subtokeniser(current, "-"); //always gives us 2 str tokens: min and max
        
        // 1. get the length of the number strings - if you don't want to go through
        // the tokenisation system, believe me this works just fine
        min_digits = subtokensbuff[0]->length;
        max_digits = subtokensbuff[1]->length;

        // 2. convert them to numbers by first taking the chars and getting the raw num
        // out of the representation via an offset = 48. The numbers are progressively multiplied by
        // a multiple of 10, to compose the actual entire number 
        for (unsigned int i = 1; i < subtokensbuff[0]->length; i++) {
            min += (subtokensbuff[0]->buff[0] - 48)*power(10, subtokensbuff[0]->length-i);
        }
        for (unsigned int i = 1; i < subtokensbuff[0]->length; i++) {
            max += (subtokensbuff[1]->buff[0] - 48)*power(10, subtokensbuff[1]->length-i);
        }

        // 3. we need to start and end where we can operate: a number with an even amount of digits
        // E.g. if we have the range 101 - 1040, there is no point in evaluating 101-999, since we will
        // never have a pair of equal halves. Instead, we straight up jump to 10^3
        // (that is 10^(digits_of_the_lower_bound + 1))
        if (min_digits % 2 != 0) {
            min = power(10, min_digits+1); // upgrading to biggest number with an even amount of digits in the range
        }
        if (max_digits % 2 != 0) { // something similar but downwards for the upper boundary
            max = power(10, max_digits)-1; // downgrading
        }

        // 4. left half must be equal to right one at all times; we must constantly check if the half we're 
        // lerping, when repeated, is still in the valid range.
        // Supposing we have a number as follows: L | R, where L is the left half and R is the right half, 
        // so that the number of digits in L equals half of the digits of L+R.
        // We know that digits can repeat ONLY when R = L. This is our starting condition. The number we start
        // lerping from is L. We now need, though, a way to get R. We must find a way to split `min` in 2 halves.
        // Being `min_digits` known, we can divide by 10 with size_t numbers *min_digits/2* times, so that we 
        // remains with the left side.
        size_t start = min;
        for (size_t i = 0; i < min_digits/2; i++) {
            start /= 10;
        } // in the end, start equals L, and we can use it for finding symmetries.
        // Just to be even more clear:
        // if we have a number abcd, it's clear that the minimum number fitting in abcd will be
        // 1000a + 100b + 10c + d, but the minimum SYMMETRICAL number will necessarily be:
        // abab = 1000a + 100b + 10a + b. To convince yourself try and see what happens if
        // from abcd we start from cdcd or a(b-1)a(b-1).
        // In the first case we can either have a number below the minimum (cd < ab) or
        // above (cd > ab), effectively losing some symmetrical ones.

        // 5. We slowly increment start and "duplicate" it over to the other side. We work as if we were on the right one.
        size_t R = start; // used to save the right side and sum it when recomposing the full symmetric number
        while (true) {
            if (min_digits % 2 != 0) { // we gotta skip uneven nums
                min_digits++;
                R = power(10, min_digits/2);
            }
            
            // notice we check also if it's greater than min, because we are not sure first ones really are.
            // E.g. min = 3234; start = 32; symmetric = 3232 < min
            size_t symmetric = R*power(10, min_digits/2) + R;
            if (symmetric < max && symmetric > min) {
                counter += symmetric ; // we have found a symmetric number that's in the range
            } else if (symmetric < min) {
                R++;
                continue;
            } else {
                break;
            }
            
            // gradually updates `min_digits` by getting the amount of bits and subtracting the most significant zeroes,
            // then divides by log2(10) and casts to int to have an int, adds 1 to complete.
            R++;
            min_digits = (int)((sizeof(counter)*8 - __builtin_clzll(counter))/log2(10)) + 1;
        }

        min = max = 0;
        current = current->next;
    }

    printf("-------- INVALID CHARS COUNT --------\n%zu\n-------- END --------", counter);

    return 0;
}