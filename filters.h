#pragma once

#include "defs.h"

void bitShuffle(
    tdata &data,
    const uint32_t shuffle
);

void byteShuffle(
    tdata &data,
    const uint32_t shuffle
);


void moveToFront(
    tdata &data
);

void moveToFront16(
    tdata &data
);

