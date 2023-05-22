#pragma once

#include <vector>
#include <cstdlib>
#include <climits>

#include "defs.h"


class BitBuff {
private:
    tdata &buf;
    uint32_t nBits = 0;

public:
    BitBuff( tdata &buf ) : buf(buf) {
        buf.clear();
    }

    size_t size() const {
        return buf.size();
    }
    uint32_t bits() const {
        return nBits;
    }
    void addBit( const tbyte bit ) {
        if( bit & ~1 ) { abort(); }
        if( nBits % CHAR_BIT == 0 ) {
            buf.push_back(bit);
            nBits = 1;
        } else {
            buf[ buf.size() - 1 ] |= bit << nBits;
            nBits++;
        }
    }

    const tbyte& operator[](const size_t i) const {
        return buf[i];
    }
    tbyte& operator[](const size_t i) {
        return buf[i];
    }

    template<typename T>
    void append(T v) {
        for(uint32_t i=0 ; i<sizeof(T) * CHAR_BIT ; i++ ) {
            addBit( (tbyte)(v & (T)1) );
            v >>= 1;
        }
    }

    template<typename T>
    void append(T v, const uint32_t maxBits) {
        if( maxBits > sizeof(T) * CHAR_BIT ) { abort(); }
        for(uint32_t i=0 ; i<maxBits ; i++ ) {
            addBit( (tbyte)(v & (T)1) );
            v >>= 1;
        }
        if( v ) { abort(); }
    }


    void append(const BitBuff &bb) {
        for(size_t i=0 ; i+1<bb.size() ; i++ ) {
            append( bb[i] ) ;
        }
        for( uint32_t i=0 ; i<bb.bits() ; i++ ) {
            addBit( (bb[bb.size()-1] >> i) & (tbyte)1u );
        }
    }

    template<typename T>
    void operator += (const T v) {
        append(v);
    }

    void operator += (const BitBuff &bb) {
        append( bb );
    }



};

