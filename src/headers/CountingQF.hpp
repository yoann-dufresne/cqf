#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <vector>
#include <cstdint>

class CountingQF 
{
    private:
        std::vector<bool> occupied;
        std::vector<bool> runend;
        std::vector<uint64_t> remainders;
        int elementSize;
    
    public:
        explicit CountingQF(int elementSize);
        explicit CountingQF(int elementSize, uint64_t sizeEstimate);
};

#endif