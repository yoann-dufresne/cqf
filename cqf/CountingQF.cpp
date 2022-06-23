#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <bitset>

#define MEM_UNIT 8

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

    uint64_t quotientLen = powerOfTwo;
    uint64_t remLen = 64 - quotientLen;

    uint64_t totalRemsLen = numberOfSlots * remLen;

    /* Blocks are made of an offset (8b) + occ (64b) 
    + run (64b) + remainders (64b * remLen) */

    uint64_t numberOfBlocks = numberOfSlots / 64;
    if (numberOfBlocks == 0)
        numberOfBlocks++;

    uint64_t filterSize = (8 * numberOfBlocks) + totalOccLen + totalRunLen + totalRemsLen;
    
    uint64_t blockByteSize = filterSize / numberOfBlocks / 8;
    memset(this -> remainderPos, 0, 64*2);


    for (int slot = 0; slot < 64; slot++)
    {
        int posBit = (slot * remLen);
        //blocks to skip
        this -> remainderPos[slot][0] = posBit / 8;
        //bits to skip
        this -> remainderPos[slot][1] = posBit % 8;
    }

    this -> qf = new uint8_t[(blockByteSize * numberOfBlocks)];

    memset(qf, 0, blockByteSize * numberOfBlocks);

    this -> filterSize = filterSize;
    this -> numberOfSlots = numberOfSlots;
    this -> numberOfBlocks = numberOfBlocks;
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
