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
        inline uint8_t getNthBitFrom(uint64_t vec, int n);
        inline void setNthBitFrom(uint64_t &vec, int n);
        inline void setNthBitToX(uint64_t &vec, int n, int x);
        inline void clearNthBitFrom(uint64_t &vec, int n);

};

#endif