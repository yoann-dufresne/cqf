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
        std::bitset<64> occupieds;
        std::bitset<64> runends;
        std::vector<uint64_t> remainders;

        explicit CountingQF();
                
        void insertValue(uint64_t el);
        int findFirstUnusedSlot(uint64_t el);
        
        int asmSelect(uint64_t val, int i);
        int asmRank(uint64_t val, int i);
};

#endif