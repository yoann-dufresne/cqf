#include <CountingQF.hpp>

#define VEC_LEN 64
#define JUMP_SIZE 8

void printbits(unsigned long number, unsigned int num_bits_to_print)
{
    if (number || num_bits_to_print > 0) {
        printbits(number >> 1, num_bits_to_print - 1);
        printf("%d", (int) number & 1);
    }
}

void printbits64(unsigned long number) {
    printbits(number, 64);
}


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
    uint64_t remLen = VEC_LEN - quotientLen;

    uint64_t totalRemsLen = numberOfSlots * remLen;

    uint64_t filterSize = totalOccLen + totalRunLen + totalRemsLen;

    /* Blocks are made of an offset (8b) + occ (64b) 
    + run (64b) + remainders (64b * remLen) */

    uint64_t blockByteSize = filterSize / 8;

    uint64_t numberOfBlocks = filterSize / blockByteSize;
    
    memset(this -> remainderPos, 0, 64*2);

    for (int slot = 0; slot < 64; slot++)
    {
        int posBit = (slot * remLen);
        //blocks to skip
        this -> remainderPos[slot][0] = posBit / 8;
        //bits to skip
        this -> remainderPos[slot][1] = posBit % 8;
    }

    this -> qf = new uint8_t[filterSize / 8];

    memset(qf, 0, filterSize /8);

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
    uint64_t slotsPerBlock = (numberOfSlots / numberOfBlocks);

    // Block corresponding to quotient
    uint64_t block = quotient / slotsPerBlock;
    uint64_t slotPos = quotient % (numberOfSlots * block);

    // Memory address of block (Same address as offset)
    uint8_t * blockAddr = qf + block * (blockByteSize);
    uint8_t * occAddr = blockAddr + 1;
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
        blockCounter += 1;
        
        blockAddr = (uint8_t *) (block * blockByteSize);
        occAddr = blockAddr + 1;
        runAddr = occAddr + JUMP_SIZE;

        occupiedsVec = (uint64_t *) occAddr;
        runendsVec = (uint64_t *) runAddr;

        occSlotsToPos = asmRank(*occupiedsVec, VEC_LEN);
        lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);
    }

    if (blockCounter == 0)

    // While we haven't hit our slot on the first block, keep searching
    while (blockCounter >= 0)
    {
        blockAddr = (uint8_t *) (block * blockByteSize);
     
        block -= 1;
        blockCounter -= 1;

        for (uint64_t remPos = 0; remPos < VEC_LEN; remPos++)
        {
            uint64_t rem = getRemFromBlock(remPos, blockAddr);

            if (valRem == rem)
                return true;
        }
    }

    return false;
}

void CountingQF::insertValue(uint64_t val)
{
    uint64_t quotient = val >> remainderLen;
    uint64_t valRem = (val << quotientLen) >> quotientLen;


    // Block corresponding to quotient
    uint64_t block = quotient / VEC_LEN;
    uint64_t slotPos = quotient % VEC_LEN;

    // Memory address of block
    uint8_t * blockAddr = qf + block * (1 + blockByteSize);
    uint8_t * occAddr = blockAddr + JUMP_SIZE;
    uint8_t * runAddr = occAddr + JUMP_SIZE;

    uint64_t * occupiedsVec = (uint64_t *) occAddr;
    uint64_t * runendsVec = (uint64_t *) runAddr;

    uint64_t occSlotsToPos = asmRank(*occupiedsVec, slotPos);
    uint64_t lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);

    if (slotPos > lastSlotInRun)
    {
        setNthBitFrom(*runendsVec, slotPos);
        setRemAtBlock(valRem, slotPos, blockAddr);
    }
    else
    {
        ++lastSlotInRun %= 64;

        int * slotInfo = findFirstUnusedSlot(lastSlotInRun, blockAddr);
        
        int blockCounter = slotInfo[1];
        int pos = slotInfo[0];

        // if it's multiblock, we have to do this
        if (blockCounter != 0)
        {
            //first block, start swapping at runend slot
            for (int lastPos = pos; lastPos > 0; lastPos--)
            {
                uint8_t * runendsAddr = blockAddr + 1 + 8;
                uint64_t * runends = (uint64_t *) runendsAddr;

                setRemAtBlock(getRemFromBlock(lastPos - 1, blockAddr), 
                    lastPos, blockAddr);

                setNthBitToX(*runends, lastPos, 
                    getNthBitFrom(*runends, lastPos - 1));
            }
        }

        blockCounter--;
        // for all blocks in between, start swapping from end to finish
        while (blockCounter > 1)
        {
            for (int lastPos = 63; lastPos > 0; lastPos--)
            {
                uint8_t * runendsAddr = blockAddr + 1 + 8;
                uint64_t * runends = (uint64_t *) runendsAddr;

                setRemAtBlock(getRemFromBlock(lastPos - 1, blockAddr), 
                    lastPos, blockAddr);

                setNthBitToX(*runends, lastPos, 
                    getNthBitFrom(*runends, lastPos - 1));
            }
            blockCounter--;
        }

        // for the last block, swap from runend until you reach original 
        // occupied slot (slotPos)
        for (uint64_t lastPos = pos; lastPos > slotPos; lastPos--)
        {
            uint8_t * runendsAddr = blockAddr + 1 + 8;
            uint64_t * runends = (uint64_t *) runendsAddr;

            setRemAtBlock(getRemFromBlock(lastPos - 1, blockAddr), 
                lastPos, blockAddr);

            setNthBitToX(*runends, lastPos, 
                getNthBitFrom(*runends, lastPos - 1));
        }
        
        delete[] slotInfo;

        setRemAtBlock(valRem, lastSlotInRun, blockAddr);

        if (getNthBitFrom(*occupiedsVec, quotient) == 1)
            clearNthBitFrom(*runendsVec, lastSlotInRun - 1);
        
        setNthBitFrom(*runendsVec, lastSlotInRun);
    }
    setNthBitFrom(*occupiedsVec, quotient);
}

int * CountingQF::findFirstUnusedSlot(uint64_t fromPos, uint8_t * &blockAddr)
{
    uint8_t * occAddr = blockAddr + JUMP_SIZE;
    uint8_t * runAddr = occAddr + JUMP_SIZE;

    uint64_t * occupieds = (uint64_t *) occAddr;
    uint64_t * runends = (uint64_t *) runAddr;

    int occSlotsToPos = asmRank(*occupieds, fromPos);
    int lastSlotInRun = asmSelect(*runends, occSlotsToPos);
    int blockCounter = 0;
    int * res = new int[2];

    while ((int) fromPos <= lastSlotInRun)
    {
        fromPos = lastSlotInRun + 1;
        occSlotsToPos = asmRank(*occupieds, fromPos);
        lastSlotInRun = asmSelect(*runends, occSlotsToPos);
        
        // if it's the first insertion, there is no runend.
        // which means this will run forever until segfault
        if (lastSlotInRun == VEC_LEN)
        {
            blockCounter++;
            blockAddr += blockByteSize;
            
            occAddr = blockAddr + 1;
            runAddr = occAddr + 8;
            
            occupieds = (uint64_t *) occAddr;
            runends = (uint64_t *) runAddr;
            
            occSlotsToPos = asmRank(*occupieds, fromPos);
            lastSlotInRun = asmSelect(*runends, occSlotsToPos);
        }
    }

    res[0] = fromPos;
    res[1] = blockCounter;
    return (res);
}

uint64_t CountingQF::getRemFromBlock(int slot, uint8_t * blockAddr)
{
    int pos = remainderPos[slot][0];
    int shiftBy = remainderPos[slot][1];

                                // 8 + 8 + 1
    uint8_t * remAddr = blockAddr + 17 + pos;

    uint64_t * rem = (uint64_t *) remAddr;
    // this line is giving an invalid read, both remaddr and shiftBy
    // are fucky
    *rem <<= shiftBy;
    *rem >>= shiftBy + (VEC_LEN - remainderLen);

    return *rem;
}

void CountingQF::setRemAtBlock(uint64_t rem, int slot, uint8_t * blockAddr)
{
    int pos = remainderPos[slot][0];
    int shiftBy = remainderPos[slot][1];

    uint8_t * remAddr = blockAddr + 17 + pos;

    // Setting first uint8
    uint8_t firstRemPart = rem >> (56 + shiftBy);

    *remAddr &= firstRemPart;

    // Removing already added part from remainder
    rem <<= shiftBy;

    // Advance one block
    remAddr += 1;
    
    // How many uint8s left, not counting first one.
    int iterations = (((remainderLen - 7) / 8) - 1);

    for (int i = 0; i < iterations; i++)
    {
        *remAddr = (uint8_t) rem >> ((56 - (i * 8)) & 0xFF);
        remAddr += 1;
    }
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
