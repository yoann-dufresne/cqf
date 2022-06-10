#include <CountingQF.hpp>

#define VEC_LEN 64
#define JUMP_SIZE 8

CountingQF::CountingQF(uint32_t powerOfTwo)
{
    /*
    * Need total remainders (h1) memory to be (x * (VEC_LEN - log2(x)))
    * aka. so len(h0) needs to be of len log2(x) bits to have enough
    * values for the possible slots.
    */

    uint64_t numberOfSlots = 1ULL << powerOfTwo;
    uint64_t totalOccLen = numberOfSlots;
    uint64_t totalRunLen = numberOfSlots;

    uint64_t quotientLen;
    
    // bsr asm instruction returns pos of highest set bit, same as log2.   
    asm ( "bsr %1, %0"
        : "=r" (quotientLen)
        : "r" (numberOfSlots)
    );

    uint64_t remLen = VEC_LEN - quotientLen;
    uint64_t totalRemsLen = numberOfSlots * remLen;

    uint64_t filterSize = totalOccLen + totalRunLen + totalRemsLen;

    /* Blocks are made of an offset (8b) + occ (64b) 
    + run (64b) + remainders (64b * remLen) */

    uint64_t blockBitSize = (sizeof(uint8_t) + VEC_LEN * (2 + remLen));
    uint64_t blockByteSize = blockBitSize / 8;

    uint64_t numberOfBlocks = filterSize / blockBitSize;

    this -> qf = new uint8_t[filterSize / JUMP_SIZE];

    this -> filterSize = filterSize;
    this -> numberOfSlots = numberOfSlots;
    this -> numberOfBlocks = numberOfBlocks;
    this -> blockBitSize = blockBitSize;
    this -> blockByteSize = blockByteSize;
    this -> quotientLen = quotientLen;
    this -> remainderLen = remLen;
}

bool CountingQF::query(uint64_t val)
{
    return false;
}

void CountingQF::insertValue(uint64_t val)
{
}

int CountingQF::findFirstUnusedSlot(uint64_t fromPos)
{
    return 1;
}

int CountingQF::asmRank(uint64_t val, int pos)
{
    // Keep only bits up to pos pos non-including.
    val = val & (((2ULL) << pos) - 1);

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int CountingQF::asmSelect(uint64_t val, int n)
{          
    uint64_t pos = 1ULL << n;

    asm("pdep %[val], %[mask], %[val]"
		: [val] "+r" (val)
		: [mask] "r" (pos));
	
    asm("tzcnt %[bit], %[index]"
        : [index] "=r" (pos)
        : [bit] "g" (val)
		: "cc");

    return (pos);
}

inline uint8_t CountingQF::getNthBitFrom(uint64_t vec, int n) {
    return (vec >> n) & 0b1;
}

inline void CountingQF::setNthBitFrom(uint64_t &vec, int n) {
    vec |= 1ULL << n;
}

inline void CountingQF::setNthBitToX(uint64_t &vec, int n, int x) {
    vec ^= (-x ^ vec) & (1ULL << n);
}

inline void CountingQF::clearNthBitFrom(uint64_t &vec, int n) {
    vec &= ~(1ULL << n);
}
