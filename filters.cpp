#include <algorithm>

#include "filters.h"
#include "bit_buff.h"

void bitShuffle(
    tdata &data,
    const uint32_t shuffle
) {
    tdata tmp;
    BitBuff bb(tmp);
    for( uint32_t i=0 ; i<shuffle ; i++ ) {
        for( size_t j=i ; j<data.size()*CHAR_BIT ; j+=shuffle ) {
            const size_t byte = j >> 3;
            const tbyte  bit  = 1 << (j&7);
            bb.addBit( (data[byte] & bit) >> (j&7) );
        }
    }
    if( tmp.size() != data.size() ) { abort(); }
    data = tmp;
}



void byteShuffle(
    tdata &data,
    const uint32_t shuffle
) {
    tdata tmp( data.size() );
    uint32_t k=0;
    for( uint32_t i=0 ; i<shuffle ; i++ ) {
        for(uint32_t j=i ; j<data.size() ; j+=shuffle ) {
            tmp[k++] = data[j];
        }
    }
    if( k != data.size() ) {
        abort();
    }
    data = tmp;
}


void moveToFront(
    tdata &data
) {
    std::vector<uint8_t> pos(256);
    for( uint32_t i=0 ; i<pos.size() ; i++ ) {
        pos[i] = i;
    }
    for( uint32_t i=0 ; i<data.size() ; i++ ) {
        const uint32_t idx = std::find( pos.cbegin() , pos.cend() , data[i] ) - pos.cbegin();
        for( uint32_t j=idx ; j>0 ; j-- ) {
            pos[j] = pos[j-1];
        }
        pos[0] = data[i];
        data[i] = (uint8_t)idx;
    }
}

