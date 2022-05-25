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

int CountingQF::asmRank(uint64_t val, int i)
{
    std::cout << val << '\n';
    val = val & ((2ULL) << i) - 1;
    std::cout << val << '\n';
    
    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int CountingQF::asmSelect(uint64_t val, int n)
{
    uint64_t i = 1ULL << n;
    
    asm("pdep %[val], %[mask], %[val]"
		: [val] "+r" (val)
		: [mask] "r" (i));
	
    asm("tzcnt %[bit], %[index]"
		: [index] "=r" (i)
		: [bit] "g" (val)
		: "cc");
	
    return (i);
}