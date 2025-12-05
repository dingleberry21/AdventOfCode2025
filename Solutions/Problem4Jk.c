#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../libs/Bnsparser.h"

/* Day 3 of the AoC 2025. Tried to be funny. Actual solution is to come. */

double nCr( int n, int k) {
    if ( k < 0 || k > n ) return 0;
    if ( k == 0 || k == n ) return 1;
    if ( k > n / 2 ) k = n - k;
    
    double res = 1;
    for ( int i = 1; i <= k; ++i ) {
        res = res * (n - i + 1) / i;
    }
    return res;
}

double func( void ) {
    FILE *fp = fopen( "Inputs/Problem4.txt", "r" );
    if ( !fp ) exit( -1 );
    
    file *F = bnsreadf( fp );
    fclose( fp );

    long total_cells = 0;
    long total_rolls = 0;
    
    line *curr_line = F->first;
    
    while ( curr_line != NULL ) {
        for ( size_t i = 0; i < curr_line->length; i++ ) {
            char c = curr_line->buff[ i ];
            
            total_cells++;
            if ( c == '@' ) {
                total_rolls++;
            }
        }
        curr_line = curr_line->next;
    }

    if ( total_cells == 0 ) {
        exit( -1 );
    }

    double p = ( double )total_rolls / total_cells;
    
    double prob_accessible = 0.0;
    
    for ( int k = 0; k < 4; k++ ) {
        // P(X=k) = nCk * p^k * (1-p)^(n-k)
        double term = nCr( 8, k ) * pow( p, k ) * pow( 1.0 - p, 8 - k );
        prob_accessible += term;
    }

    return total_rolls * prob_accessible;

}

int main() {
    double prediction = func();
    
    printf( "Predicted Accessible Rolls: %.0f\n", prediction );
    return 0;
}