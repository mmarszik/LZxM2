#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <unordered_set>
#include <algorithm>
#include "LZxM00Pack.h"

#include "rnd.h"
#include "filters.h"


struct Opt {
    uint32_t dictSize1  = 12;
    uint32_t maxWord1   =  2;
    uint32_t minWord1   =  2;
    uint32_t byteShuff1 =  3;
    uint32_t bitShuff1  =  8;
    bool     moveToFront1 = false;
    bool isTwo = false;
    uint32_t dictSize2 = 12;
    uint32_t maxWord2 = 2;
    uint32_t minWord2 = 2;

    void chaos() {
        switch( myRand() % (isTwo ? 10 : 7) ) {
            case 0:
                myChaos( dictSize1 , 2 , 16 );
                break;
            case 1:
                myChaos( maxWord1  , 1 ,  6 );
                break;
            case 2:
                myChaos( minWord1  , 1 , 32 );
                break;
            case 3:
                myChaos( byteShuff1 , 1 , 32 );
                break;
            case 4:
                myChaos( bitShuff1 , 1 , 64 );
                break;
            case 5:
                moveToFront1 = !moveToFront1;
                break;
            case 6:
                isTwo = !isTwo;
                break;
            case 7:
                myChaos( dictSize2 , 2 , 16 );
                break;
            case 8:
                myChaos( maxWord2  , 1 ,  6 );
                break;
            case 9:
                myChaos( minWord2  , 1 , 32 );
                break;
        }
    }

    bool operator == (const Opt &other) const {
        if( isTwo != other.isTwo ) {
            return false;
        }
        return dictSize1 == other.dictSize1 &&
                maxWord1 == other.maxWord1 &&
                minWord1 == other.minWord1 &&
                byteShuff1 == other.byteShuff1 &&
                bitShuff1 == other.bitShuff1 &&
                moveToFront1 == other.moveToFront1 &&
                (
                    isTwo == false
                    ||
                    (
                        dictSize2 == other.dictSize2 &&
                        maxWord2 == other.maxWord2 &&
                        minWord2 == other.minWord2
                    )
        );
    }

};

struct hash_fn
{
    std::size_t operator() (const Opt &opt) const
    {
        size_t h = 13412;
        h += opt.dictSize1;
        h *= 1331;
        h += opt.maxWord1;
        h *= 1001;
        h += opt.minWord1;
        h *= 371;
        h += opt.byteShuff1;
        h *= 291;
        h += opt.bitShuff1;
        h *= 173;
        if( opt.moveToFront1 ) {
            h += 13321;
        }
        if( ! opt.isTwo ) {
            return h;
        }
        h += opt.dictSize2;
        h *= 2317;
        h += opt.maxWord2;
        h *= 201;
        h += opt.minWord2;
        h *= 171;

        return h;
    }
};


using TOptBuff = std::unordered_set<Opt,hash_fn>;

int main( int argc, char *argv[] ) {
    srand(1235);
    std::ifstream inpFile( argv[1], std::ios::binary );
    tdata data(std::istreambuf_iterator<char>(inpFile), {});
    std::cout << "read from '" << argv[1] << "' " << data.size() << " bytes." << std::endl;
    inpFile.close();
    tdata output;

    Opt curr,best;
    size_t bestSize = std::numeric_limits<size_t>::max();
    double bestRatio = 0;

    TOptBuff optSet;

//    for( uint32_t i=0 ; i<1000 ; i++ ) {
//        curr.chaos();
//    }


    for( uint32_t loop=0 ; true ; loop++ ) {

        if( loop > 0 ) {
            curr.chaos();
        }

        if( optSet.count( curr ) > 0 ) {
            std::cout << "loop: " << loop << " continue" << std::endl;
            continue;
        }
        optSet.insert( curr );

        const time_t start = time(NULL);

        std::vector<tbyte> output = data;
        if( curr.byteShuff1 > 1 ) {
            byteShuffle( output , curr.byteShuff1 );
        }
        if( curr.bitShuff1 > 1 ) {
            bitShuffle(output,curr.bitShuff1 );
        }
        if( curr.moveToFront1 ) {
            moveToFront( output );
        }

        {
            tdata tmp;
            LZxM00Pack::packH(output,tmp,curr.dictSize1,curr.maxWord1,curr.minWord1);
            output = tmp;
        }

        if( curr.isTwo ) {
            tdata tmp;;
            LZxM00Pack::packH(output,tmp,curr.dictSize2,curr.maxWord2,curr.minWord2);
            output = tmp;
        }

        if( bestSize >= output.size() ) {
            bestSize = output.size();
            best = curr;
            std::cout << "loop: " << loop << std::endl;
            std::cout << "dictSize1: " << best.dictSize1 << std::endl;
            std::cout << "maxWord1: " << best.maxWord1 << std::endl;
            std::cout << "minWord1: " << best.minWord1 << std::endl;
            std::cout << "byteShuff: " << best.byteShuff1 << std::endl;
            std::cout << "bitShuff: " << best.bitShuff1 << std::endl;
            std::cout << "moveToFront1: " << best.moveToFront1 << std::endl;
            std::cout << "isTwo: " << best.isTwo << std::endl;
            std::cout << "dictSize2: " << best.dictSize2 << std::endl;
            std::cout << "maxWord2: " << best.maxWord2 << std::endl;
            std::cout << "minWord2: " << best.minWord2 << std::endl;

            std::cout << "time: " << time(NULL) - start << std::endl;
            std::cout << "compres: " << output.size() << std::endl;
            const double ratio = ((double)data.size() - output.size()) / data.size() * 100;
            std::cout << ratio << "%" << std::endl;
            bestRatio = ratio;
        } else {
            if( rand() % 3 == 0 ) {
                curr = best;
            }
            std::cout << "loop: " << loop << std::endl;
            std::cout << "time: " << time(NULL) - start << std::endl;
            std::cout << "compres: " << output.size() << std::endl;
            const double ratio = ((double)data.size() - output.size()) / data.size() * 100;
            std::cout << ratio << "%" << " / " << bestRatio << "%" << std::endl;
        }
        std::cout << "----------------------------------------------" << std::endl ;

    }





//    if( byteShuffle1 > 0 ) {
//        makeByteShuffle(data,byteShuffle1);
//    }
//    if( useMTF ) {
//        moveToFront( data );
//    }
//    if( byteShuffle2 > 0 ) {
//        makeByteShuffle(data,byteShuffle2);
//    }
//    const uint32_t bitStat = lzStat(data);
//    const double stat = bitStat / 8;
//    std::cout << "stat: " << bitStat << " " << (data.size() - stat) / data.size() * 100 << "%" << std::endl;
    return 0;
}















#ifdef __UNDEFINDE__

//const uint32_t dictSize    =    20;
//const uint32_t maxFoudSize =     4;
//const uint32_t byteShuffle1 =    3;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;
//63.8% 32.13% 29238s



//const uint32_t dictSize    =    20;
//const uint32_t maxFoudSize =     4;
//const uint32_t byteShuffle1 =    0;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;
//100% 33.74% 23708s



//const uint32_t dictSize    =    20;
//const uint32_t maxFoudSize =     5;
//const uint32_t byteShuffle1 =    0;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;
//95.52% 31.72% 30738s


//const uint32_t dictSize    =    21;
//const uint32_t maxFoudSize =     5;
//const uint32_t byteShuffle1 =    0;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;
//62.9% 29.08% 34428s

//const uint32_t dictSize    =    19;
//const uint32_t maxFoudSize =     4;
//const uint32_t byteShuffle1 =    0;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;
//25.68% 28.93% 5688s






const uint32_t dictSize    =    10;
const uint32_t maxFoudSize =     4;
const uint32_t byteShuffle1 =    0;
const bool useMTF          =  false;
const uint32_t byteShuffle2 =    0;


uint32_t lzFind(
    const std::vector<uint8_t> &data,
    const uint32_t data_start,
    const std::vector<uint8_t> &dict,
    const uint32_t dict_start
) {
    uint32_t i = data_start;
    uint32_t j = dict_start;
    while( i < data.size() && j < dict.size() && data[i] == dict[j] ) {
        i++;
        j++;
    }
    return i - data_start;
}

void lzFindBest(
    const std::vector<uint8_t> &data,
    const uint32_t data_start,
    const std::vector<uint8_t> &dict,
    uint32_t &pos,
    uint32_t &size
) {
    size = 0;
    for( uint32_t i=0 ; i<dict.size() ; i++ ) {
       const uint32_t tmp = lzFind(data,data_start,dict,i);
       if( tmp > size ) {
           size = tmp;
           pos = i;
           if( size >= (1u << maxFoudSize) ) {
               size = (1u << maxFoudSize);
               break;
           }
        }
    }
}

uint32_t mostBit( uint32_t v ) {
    uint32_t i=0;
    while( v ) {
        v >>= 1;
        i++;
    }
    return i;
}

void lzAdd(
    std::vector<uint8_t> &dict,
    const uint8_t c
) {
//    dict.insert( dict.begin() , c );
//    if( dict.size() > ( 1 << dictSize)  ) {
//        dict.resize( 1 << dictSize );
//    }

    dict.insert( dict.end() , c );
    while( dict.size() > ( 1 << dictSize)  ) {
        dict.erase( dict.begin() );
    }

}

uint32_t lzStat(
    const std::vector<uint8_t> &data
) {
    const time_t start = time(NULL);
//    uint32_t showChunk = data.size() / 5000;
    time_t lastShow = time(NULL);
    std::vector<uint8_t> dict;
    uint32_t stat = 0;

    for( uint32_t i=0 ; i<data.size() ;  ) {
        uint32_t pos=0,size;
        lzFindBest(data,i,dict,pos,size);
        const uint32_t currDictSize = mostBit( dict.size()-1 );
        if( size*8 > 1 + currDictSize + maxFoudSize ) {
            stat += 1 + currDictSize + maxFoudSize;
            for( uint32_t j=0 ; j<size ; j++ ) {
                lzAdd(dict,data[i++]);
            }
        } else {
            lzAdd(dict,data[i]);
            i += 1;
            stat += 1 + 8;
        }
        if( i == data.size()-1 || ( i > 0 && time(NULL) - lastShow >= 10 ) ) {
            std::cout << std::round( 10000.0 / data.size() * i ) / 100.0 << "%" << " " << std::round(10000.0 * (i - stat/8.0) / i ) / 100.0  << "% " << (time(NULL)-start) << "s" << std::endl;
            lastShow = time(NULL);
        }
//        std::cout << std::string(dict.begin(), dict.end()) << " ";
//        std::cout << pos << " " << size << std::endl;
    }
    return stat;

}


//const uint32_t dictSize    =    20;
//const uint32_t maxFoudSize =     4;
//const uint32_t byteShuffle1 =    3;
//const bool useMTF          = false;
//const uint32_t byteShuffle2 =    0;


using tKey = uint32_t;
using tHashMap = std::unordered_multimap<tKey,uint32_t>;
using mapIter = tHashMap::iterator;

class OutBuff {
private:
    std::vector<uint8_t> out;
    uint8_t bits = 0;

private:
    void addBit( uint8_t bit ) {

    }




};

static tKey strHashKey(
    const std::vector<uint8_t> &data,
    const uint32_t             iData,
    const uint32_t             minFound
) {
    tKey hash = 0;
    for( uint32_t i=iData ; i<iData+minFound && i<data.size() ; i++ ) {
        hash = hash * 313451 + 71233u + data[i];
    }
    return hash;
}

static uint32_t lzSize(
    const std::vector<uint8_t> &data,
    const uint32_t             iData,
    const std::vector<uint8_t> &dict,
    const uint32_t             iDict
) {
    uint32_t size = 0;
    while(
        size+iData<data.size() &&
        size+iDict<dict.size() &&
        data[size+iData] == dict[size+iDict]
    ) {
        size++;
    }
    return size;
}

static void lzFindBest(
    const std::vector<uint8_t> &dict,
    const tHashMap             &hashMap,
    const std::vector<uint8_t> &data,
    const uint32_t             iData,
    const uint32_t             minFound,
    uint32_t                   &pos,
    uint32_t                   &size
) {
    size = 0;
    const tKey key = strHashKey(data,iData,minFound);
    auto range = hashMap.equal_range(key);
    for (auto i = range.first; i != range.second; ++i) {
        // pos == i->second;
        const uint32_t tmp = lzSize(data,iData,dict,i->second);
    }


}

static uint32_t lzStat(
    const std::vector<uint8_t> &data,
    uint32_t dictSize,
    uint32_t maxFound,
    const uint32_t hasFound
) {
    const time_t start = time(NULL);
    time_t lastShow = start;

    dictSize = 1u << dictSize;
    maxFound = 1u << maxFound;

    std::vector<uint8_t> dict;
    uint32_t dictHead = 0;
    tHashMap hash;

    uint32_t stat = 0;

    for( uint32_t i=0 ; i<data.size() ;  ) {
        uint32_t pos=0,size;
    }


//    uint32_t showChunk = data.size() / 5000;

//    for( uint32_t i=0 ; i<data.size() ;  ) {
//        uint32_t pos=0,size;
//        lzFindBest(data,i,dict,pos,size);
//        const uint32_t currDictSize = mostBit( dict.size()-1 );
//        if( size*8 > 1 + currDictSize + maxFoudSize ) {
//            stat += 1 + currDictSize + maxFoudSize;
//            for( uint32_t j=0 ; j<size ; j++ ) {
//                lzAdd(dict,data[i++]);
//            }
//        } else {
//            lzAdd(dict,data[i]);
//            i += 1;
//            stat += 1 + 8;
//        }
//        if( i == data.size()-1 || ( i > 0 && time(NULL) - lastShow >= 10 ) ) {
//            std::cout << std::round( 10000.0 / data.size() * i ) / 100.0 << "%" << " " << std::round(10000.0 * (i - stat/8.0) / i ) / 100.0  << "% " << (time(NULL)-start) << "s" << std::endl;
//            lastShow = time(NULL);
//        }
////        std::cout << std::string(dict.begin(), dict.end()) << " ";
////        std::cout << pos << " " << size << std::endl;
//    }

    return stat;

}






int main( int argc, char *argv[] ) {
    std::ifstream inpFile( argv[1], std::ios::binary );
    std::vector<uint8_t> data(std::istreambuf_iterator<char>(inpFile), {});
    std::cout << "read from '" << argv[1] << "' " << data.size() << " bytes." << std::endl;
    inpFile.close();

//    if( byteShuffle1 > 0 ) {
//        makeByteShuffle(data,byteShuffle1);
//    }
//    if( useMTF ) {
//        moveToFront( data );
//    }
//    if( byteShuffle2 > 0 ) {
//        makeByteShuffle(data,byteShuffle2);
//    }
//    const uint32_t bitStat = lzStat(data);
//    const double stat = bitStat / 8;
//    std::cout << "stat: " << bitStat << " " << (data.size() - stat) / data.size() * 100 << "%" << std::endl;
    return 0;
}

#endif
