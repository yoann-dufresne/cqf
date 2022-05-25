#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <cstdint>
#include <bitset>
#include <vector>
#include <cmath>

class CountingQF 
{   
    public:
        uint32_t elementSize;
        uint32_t quotientSize;
        uint32_t remainderSize;
        
        uint64_t occupied;
        uint64_t runend;
        std::vector<uint64_t> remainders;

        explicit CountingQF(uint32_t elementSize);
        
        explicit CountingQF(uint32_t elementSize, uint64_t vecSizeEstimate);
        
        void insertValue(uint64_t el);
        int findFirstUnusedSlot(uint64_t el);
        
        int asmSelect(uint64_t val, int i);
        int asmRank(uint64_t val, int i);
};

#endif