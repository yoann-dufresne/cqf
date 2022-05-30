#include <CountingQF.hpp>

#define DEFAULT_VEC_LEN 64

CountingQF::CountingQF()
{
    this -> remainders = std::vector<uint64_t>(DEFAULT_VEC_LEN, 0);
    occupieds = std::bitset<64>{0b0};
    runends = std::bitset<64>{0b0};
}


int CountingQF::asmRank(uint64_t val, int i)
{
    val = val & ((2ULL) << i) - 1;

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
