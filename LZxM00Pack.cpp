#include <algorithm>

#include "LZxM00Pack.h"

#include "bit_buff.h"


static void search(
    const tdata    &input,
    const size_t   idxInput,
    const uint32_t dictSize,   //max dict size + 2^maxWord
    const uint32_t maxWord,    //2^N max of word
    uint32_t       &pos,
    uint32_t       &size
) {
    if( idxInput == 0 ) {
        return;
    }
    size_t i = idxInput;
    do {
        i--;
        size_t j=0;
        while(
            j<maxWord               &&  //to optimize: maxWord+minWord -> 0==minWord
            j+i<idxInput            &&
            j+idxInput<input.size() &&
            input[j+i] == input[j+idxInput]
        ) {
            j++;
        }
        if( size < j ) {
            size = j;
            pos  = idxInput - i;
        }
    } while( i > 0 && idxInput - i < dictSize );
}


void LZxM00Pack::pack(
    const tdata &input,
    tdata       &output,
    uint32_t    dictSize,    //max dict size + 2^maxWord
    uint32_t    maxWord,     //2^N max of word
    uint32_t    minWord      //The min szie of searching word allowed add
) {
    BitBuff bitBuf(output);

//    bitBuf += (uint32_t)input.size();
//    bitBuf += dictSize;
//    bitBuf += maxWord;
//    bitBuf += minWord;

    for( size_t i=0 ; i<input.size() ; ) {
        uint32_t pos  = 0;
        uint32_t size = 0;
        search( input , i , 1u<<dictSize , (1u<<maxWord)+minWord-1 , pos , size );
        if( size >= minWord ) {
            bitBuf.addBit( (tbyte)1 );
            bitBuf.append( pos-1        , dictSize );
            bitBuf.append( size-minWord , maxWord );
            i += size;
        } else {
            bitBuf.addBit( (tbyte)0 );
            bitBuf += input[i];
            i += 1;
        }
    }

}


static size_t fHash(
    const tdata  &data,
    const size_t idxData,
    uint32_t     minWord
) {
    size_t hash = 0;
    for( uint32_t i=idxData ; i<idxData+minWord ; i++ ) {
        hash *= 7967777;
        hash += data[i] + 7490537;
    }
    return hash;
}

using TEntry = std::vector<uint32_t>; //positions per one hash
using THash = std::vector< TEntry >;

static void addToHash(
    THash        &hashT,
    const tdata  &data,
    const size_t idxData,
    uint32_t     minWord
) {
    const size_t hashV = fHash( data , idxData-minWord , minWord );
    TEntry &entry = hashT[ hashV % hashT.size() ];
    entry.push_back( idxData - minWord );
}

static void removeFromHash(
    THash        &hashT,
    const tdata  &data,
    const size_t idxData,
    uint32_t     minWord
) {
    const size_t hashV = fHash( data , idxData , minWord );
    TEntry &entry = hashT[ hashV % hashT.size() ];
    if( std::find( entry.begin() , entry.end() , idxData ) == entry.end() ) { abort(); }
    entry.erase( std::find( entry.begin() , entry.end() , idxData ) );
}



void LZxM00Pack::packH(
    const tdata &input,
    tdata       &output,
    uint32_t    dictSize,    //max dict size + 2^maxWord
    uint32_t    maxWord,     //2^N max of word
    uint32_t    minWord      //The min szie of searching word allowed add
) {
    BitBuff bitBuf(output);
    THash hash( 1 << dictSize );

    bitBuf += (uint32_t)input.size();
    bitBuf += dictSize;
    bitBuf += maxWord;
    bitBuf += minWord;

    for( size_t i=0 ; i<input.size() ; ) {
        uint32_t pos  = 0;
        uint32_t size = 0;
        size_t inc;
        search( input , i , 1u<<dictSize , (1u<<maxWord)+minWord-1 , pos , size );
        if( size >= minWord ) {
            bitBuf.addBit( (tbyte)1 );
            bitBuf.append( pos-1        , dictSize );
            bitBuf.append( size-minWord , maxWord );
            inc = size;
        } else {
            bitBuf.addBit( (tbyte)0 );
            bitBuf += input[i];
            inc = 1;
        }
        while( inc > 0 ) {
            i++;
            if( i >= minWord ) {
                addToHash( hash , input , i , minWord );
            }
            if( i > (1u<<dictSize) ) {
                removeFromHash( hash , input , i - (1u<<dictSize) - 1 , minWord );
            }
            inc--;
        }
    }

}

