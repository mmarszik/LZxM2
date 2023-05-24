#pragma once

#include <cinttypes>

int myRand();
uint32_t mySrand();
void myRnd( uint32_t &v, const uint32_t min , const uint32_t max );
void myChaos( uint32_t &v, const uint32_t min , const uint32_t max );
