#pragma once

#include "defs.h"

class LZxM00Pack {

public:
    static void pack(
        const tdata &input,
        tdata       &output,
        uint32_t    dictSize=12,   //max dict size + 2^maxWord
        uint32_t    maxWord=4,     //2^N max of word
        uint32_t    minWord=2      //The min szie of searching word allowed add
    );

    static void packH(
        const tdata &input,
        tdata       &output,
        uint32_t    dictSize=12,   //max dict size + 2^maxWord
        uint32_t    maxWord=4,     //2^N max of word
        uint32_t    minWord=2      //The min szie of searching word allowed add
    );
};

