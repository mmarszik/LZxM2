#include <random>
#include <cstdlib>
#include "rnd.h"

using TRnd = std::ranlux48;

static TRnd rnd;

uint32_t mySrand() {
    const uint32_t seed = std::random_device()();
    rnd.seed( seed );
    return seed;
}

int myRand() {
    return rnd();
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
