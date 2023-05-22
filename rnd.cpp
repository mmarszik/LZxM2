#include <cstdlib>
#include "rnd.h"


int myRand() {
    static uint32_t x = 3;
    for( uint32_t i=0 ; i<x+6 ; i++ ) {
        rand();
    }
    x += 2;
    if( x > 30 ) {
        x = 3;
    }
    return rand();
}


void myRnd( uint32_t &v, const uint32_t min , const uint32_t max ) {
    const uint32_t cpy = v;
    do {
        v = myRand() % (max - min + 1) + min;
    } while( v == cpy );
}

void myChaos( uint32_t &v, const uint32_t min , const uint32_t max ) {
    if( myRand() % 2 == 0 ) {
        if( v == max ) {
            v -- ;
        } else if( v == min ) {
            v ++ ;
        } else if( myRand() % 2 ) {
            v -- ;
        } else {
            v ++ ;
        }
    } else {
        const uint32_t cpy = v;
        do {
            v = myRand() % (max - min + 1) + min;
        } while( v == cpy );
    }
}
