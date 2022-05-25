#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>


class CountingQF 
{   
    public:
        uint64_t occupieds;
        uint64_t runends;
        uint64_t remainders[];

        explicit CountingQF();
        
        explicit CountingQF(uint64_t remaindersLen);
        
        bool query(uint64_t val);

        int findFirstUnusedSlot(int pos);
        void insertValue(uint64_t val);
        
        int asmSelect(uint64_t val, int i);
        int asmRank(uint64_t val, int i);

        uint8_t bitAt(uint64_t x, int pos);
};

#endif