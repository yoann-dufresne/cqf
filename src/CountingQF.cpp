#include <CountingQF.hpp>

#define DEFAULT_VEC_LEN 2048

CountingQF::CountingQF(uint32_t elSize)
{
    this -> elementSize = elSize;

    this -> remainders = std::vector<uint64_t>(DEFAULT_VEC_LEN, 0);
    this -> occupied = std::vector<bool>(DEFAULT_VEC_LEN, false);
    this -> runend = std::vector<bool>(DEFAULT_VEC_LEN, false);

    this -> quotientSize = std::log2((double) DEFAULT_VEC_LEN);
    this -> remainderSize = elSize - quotientSize;
}

CountingQF::CountingQF(uint32_t elSize, uint64_t vecSizeEstimate)
{
    this -> elementSize = elSize;

    this -> remainders = std::vector<uint64_t>(vecSizeEstimate, 0);
    this -> occupied = std::vector<bool>(vecSizeEstimate, false);
    this -> runend = std::vector<bool>(vecSizeEstimate, false);

    this -> quotientSize = std::log2(vecSizeEstimate);
    this -> remainderSize = elSize - quotientSize;
}