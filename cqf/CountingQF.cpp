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

    uint64_t blockByteSize = filterSize / 8;

    uint64_t numberOfBlocks = filterSize / blockByteSize;

    this -> qf = new uint8_t[filterSize / 8];

    this -> filterSize = filterSize;
    this -> numberOfSlots = numberOfSlots;
    this -> numberOfBlocks = numberOfBlocks;
    this -> blockByteSize = blockByteSize;
    this -> quotientLen = quotientLen;
    this -> remainderLen = remLen;
}

bool CountingQF::query(uint64_t val)
{
    uint64_t quotient = val >> remainderLen;
    uint64_t valRem = (val << quotientLen) >> quotientLen;

    // Block corresponding to quotient
    uint64_t block = quotient / blockByteSize;
    uint64_t slotPos = quotient % blockByteSize;

    // Memory address of block (Same address as offset)
    uint8_t * blockAddr = qf + block * (1 + blockByteSize);
    uint8_t * occAddr = blockAddr + JUMP_SIZE;
    uint8_t * runAddr = occAddr + JUMP_SIZE;

    uint64_t * occupiedsVec = (uint64_t *) occAddr;
    uint64_t * runendsVec = (uint64_t *) runAddr;

    if (getNthBitFrom(*occupiedsVec, slotPos) == 0)
        return false;
    
    uint64_t occSlotsToPos = asmRank(*occupiedsVec, slotPos);
    uint64_t lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);

    int blockCounter = 0;

    // When we select runends up to slotPos above, if we get a 64, it means
    // the runend is not in this block, which means we need to jump to 
    // the next one.
    while (lastSlotInRun == 64)
    {
        block += 1;
        
        blockAddr = (uint8_t *) ((block) * (1 + blockBitSize));
        occAddr = blockAddr + JUMP_SIZE;
        runAddr = occAddr + JUMP_SIZE;

        occupiedsVec = (uint64_t *) occAddr;
        runendsVec = (uint64_t *) runAddr;

        occSlotsToPos = asmRank(*occupiedsVec, VEC_LEN);
        lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);
    }

    // How many uint8_t's do we need in our remainder
    int iterations = remainderLen + 7 / 8;
    
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
