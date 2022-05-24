#include <CountingQF.hpp>

CountingQF::CountingQF(int elSize)
{
    this -> elementSize = elSize;
    this -> remainders = std::vector<uint64_t>(0);
    this -> occupied = std::vector<bool>(false);
    this -> runend = std::vector<bool>(false);
}

CountingQF::CountingQF(int elSize, uint64_t sizeEstimate)
{
    this -> elementSize = elSize;
    this -> remainders = std::vector<uint64_t>(sizeEstimate, 0);
    this -> occupied = std::vector<bool>(sizeEstimate, false);
    this -> runend = std::vector<bool>(sizeEstimate, false);
}