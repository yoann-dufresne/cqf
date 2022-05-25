#include <CountingQF.hpp>

#define DEFAULT_VEC_LEN 64

CountingQF::CountingQF(uint32_t elSize)
{
    this -> elementSize = elSize;

    this -> remainders = std::vector<uint64_t>(DEFAULT_VEC_LEN, 0);
    occupied = 0ULL;
    runend = 0ULL;

    this -> quotientSize = std::log2((double) DEFAULT_VEC_LEN);
    this -> remainderSize = elSize - quotientSize;
}

CountingQF::CountingQF(uint32_t elSize, uint64_t vecSizeEstimate)
{
    this -> elementSize = elSize;

    this -> remainders = std::vector<uint64_t>(vecSizeEstimate, 0);
    occupied = 0ULL;
    runend = 0ULL;

    this -> quotientSize = std::log2(vecSizeEstimate);
    this -> remainderSize = elSize - quotientSize;
}

int CountingQF::asmRank(uint64_t val, int i)
{
    // Keep only bits up to pos i non-including, as explained below.
    val = val & ((2ULL) << i) - 1;

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

// Remove this later to avoid clutter! :)
/* i=3
* 1101101 (val)
* 0000010 (2ULL)
* 0010000 (2ULL<<i)
* 0001111 (2ULL<<i) - 1
* 0001101 (val & ...)
*/

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
