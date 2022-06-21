#include <CountingQF.hpp>
#include <bitset>

#define VEC_LEN 64
#define JUMP_SIZE 8

void printbits(unsigned long number, unsigned int num_bits_to_print)
{
    if (num_bits_to_print > 0) {
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
    uint64_t quotient = val >> remainderLen;
    uint64_t valRem = (val << quotientLen) >> quotientLen;

    // Block corresponding to quotient
    uint64_t block = quotient / VEC_LEN;
    uint64_t slotPos = quotient;
    if (block != 0)
        slotPos = quotient % (numberOfSlots * block);

    // Memory address of block (Same address as offset)
    uint8_t * blockAddr = qf + block * (blockByteSize);
    uint8_t * occAddr = blockAddr + 1;
    uint8_t * runAddr = occAddr + 8;

    uint64_t * occupiedsVec = (uint64_t *) occAddr;
    uint64_t * runendsVec = (uint64_t *) runAddr;

    if (getNthBitFrom(*occupiedsVec, slotPos) == 0)
        return false;
    
    uint64_t occSlotsToPos = asmRank(*occupiedsVec, slotPos);
    uint64_t lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);

    int blockCounter = 0;

    // When we select runends up to slotPos above, if we get a 64, it means
    // the runend is not in this block, which means we need to jump to 
    // the next one. && block != (numberOfBlocks - 1)
    while (lastSlotInRun == 64 && block != (numberOfBlocks - 1))
    {
        block += 1;
        blockCounter += 1;
        
        blockAddr = qf + (block * blockByteSize);
        occAddr = blockAddr + 1;
        runAddr = occAddr + 8;

        occupiedsVec = (uint64_t *) occAddr;
        runendsVec = (uint64_t *) runAddr;

        occSlotsToPos = asmRank(*occupiedsVec, VEC_LEN);
        lastSlotInRun = asmSelect(*runendsVec, occSlotsToPos);
    }

    if (blockCounter == 0)

    // While we haven't hit our slot on the first block, keep searching
    while (blockCounter >= 0)
    {
        
        for (uint64_t remPos = 0; remPos < VEC_LEN; remPos++)
        {
            uint64_t rem = getRemFromBlock(remPos, blockAddr);

            if (valRem == rem)
                return true;
        }

        blockCounter -= 1;
        blockAddr -= blockByteSize;
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
    uint8_t * blockAddr = qf + (block * blockByteSize);
    uint8_t * occAddr = blockAddr + 1;
    uint8_t * runAddr = blockAddr + 9;

    uint64_t * occupiedsVec = (uint64_t *) occAddr;
    uint64_t * runendsVec = (uint64_t *) runAddr;
/*    
    std::cout << "\nOccupieds and runends before insertion" << std::endl;
    printbits64(*occupiedsVec);
    std::cout << std::endl;
    printbits64(*runendsVec);
    std::cout << std::endl;
*/  

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
        if (blockCounter > 1)
        {
            uint8_t * runendsAddr = blockAddr + 1 + 8;
            uint64_t * runends = (uint64_t *) runendsAddr;
            
            //first block, start swapping at runend slot
            for (int lastPos = pos; lastPos > 0; lastPos--)
            {
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
        // occupied slot (slotPos), do it only 
        // if it was a multiblock insertion
        if (blockCounter > 0)
        {
            for (uint64_t lastPos = pos; lastPos > slotPos; lastPos--)
            {
                uint8_t * runendsAddr = blockAddr + 1 + 8;
                uint64_t * runends = (uint64_t *) runendsAddr;

                setRemAtBlock(getRemFromBlock(lastPos - 1, blockAddr), 
                    lastPos, blockAddr);

                setNthBitToX(*runends, lastPos, 
                    getNthBitFrom(*runends, lastPos - 1));
            }
        }
        delete[] slotInfo;

        setRemAtBlock(valRem, lastSlotInRun, blockAddr);

        if (getNthBitFrom(*occupiedsVec, quotient) == 1)
            clearNthBitFrom(*runendsVec, lastSlotInRun - 1);
        
        setNthBitFrom(*runendsVec, lastSlotInRun);
    }
    setNthBitFrom(*occupiedsVec, quotient);

/*    
    std::cout << "\nOccupieds and runends after insertion" << std::endl;
    printbits64(*occupiedsVec);
    std::cout << std::endl;
    printbits64(*runendsVec);
    std::cout << std::endl;
*/
}

int * CountingQF::findFirstUnusedSlot(uint64_t fromPos, uint8_t * &blockAddr)
{
    uint8_t * occAddr = blockAddr + 1;
    uint8_t * runAddr = occAddr + 8;

    uint64_t * occupieds = (uint64_t *) occAddr;
    uint64_t * runends = (uint64_t *) runAddr;

    int occSlotsToPos = asmRank(*occupieds, fromPos);
    int lastSlotInRun = asmSelect(*runends, occSlotsToPos);
    int blockCounter = 0;
    int * res = new int[2];

    while ((int) fromPos < lastSlotInRun)
    {
        fromPos = lastSlotInRun + 1;
 
        occSlotsToPos = asmRank(*occupieds, fromPos);
        lastSlotInRun = asmSelect(*runends, occSlotsToPos);
        
        // if it's the first insertion, there is no runend.
        // which means this will run forever until segfault
        // so we have to check for occSlotsToPos
        if (lastSlotInRun == VEC_LEN && occSlotsToPos != 0)
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
    int uintOffset = remainderPos[slot][0];
    int bitOffset = remainderPos[slot][1];

    printf("uintOffset %d\n bitOffset %d\n", uintOffset, bitOffset);

                                // 1 + 8 + 8
                                // offset + occ + run to pos
    uint8_t * remAddr = blockAddr + 17 + uintOffset;

    uint64_t res = 0;

    std::cout << "Remainder: " << std::bitset<64>(*((uint64_t *) remAddr)) << std::endl;
    uint8_t firstPartMask = (1ULL << (8 - bitOffset)) - 1;
    printf("mask1 %d\n", firstPartMask);

    res = *remAddr & firstPartMask;
    res <<= 8;

    std::cout << "Read first byte, res: " << std::bitset<64>(res) << std::endl;

    int nbOfUints = (remainderLen - (8 - bitOffset)) / 8;
    int nbOfEndBits = (remainderLen - (8 - bitOffset)) % 8;

    printf("nbOfUints %d\n nbOfEndBits %d\n", nbOfUints,nbOfEndBits);

    while (nbOfUints > 0)
    {
        remAddr += 1;
        res |= *remAddr;
        res <<= 8;
        std::cout << "Read next byte, res: " << std::bitset<64>(res) << std::endl;
        std::cout << "RemAddr: " << std::bitset<8>(*remAddr) << std::endl;
        nbOfUints--;
    }

    remAddr += 1;
    res |= ((*remAddr >> (8 - nbOfEndBits))) << (8 - nbOfEndBits);
    
    //res <<= bitOffset;
    
    if (quotientLen > 8)
        res <<= bitOffset ? bitOffset : 8;
    else
        res <<= bitOffset;
    
    
    std::cout << "shifted last byte, res: " << std::bitset<64>(res) << std::endl;
    std::cout << "RemAddr: " << (*remAddr >> (8 - nbOfEndBits)) << std::endl;
    std::cout << std::bitset<64>(res) << std::endl;
    return res;
}
/*
11111111111111111111111111
11111111111111111111111111

1111111111111111111111
1111111111111111111111

11111111111111111111111111
11111111111111111111111111

11111111111111111111
11111111111111111111
111111111111111111111111
11111111111111111111111111111111
11111111111111111111111111111111
111111111111111111111111
11111111111111111111
11111111111111111111
111111111111111111111111
*/
void CountingQF::setRemAtBlock(uint64_t rem, int slot, uint8_t * blockAddr)
{
    int pos = remainderPos[slot][0];
    int shiftBy = remainderPos[slot][1];

    printf("pos %d\n", pos);
    printf("bits %d\n", shiftBy);
                                // 1 + 8 + 8
                                // offset + occ + run to pos
    uint8_t * remAddr = blockAddr + 17 + pos;

    *remAddr = 0b1;

    uint8_t firstRemPart = rem >> (56 + shiftBy);

    // Setting first uint8
    *remAddr |= firstRemPart;
    //*remAddr |= 0xff;

    // Removing already added part from remainder
    rem <<= shiftBy;

    // Advance one block
    //remAddr += 1;
    
    // How many uint8s left
    /*
    int iterations = (((remainderLen - 7) / 8));
    printf("%d\n", iterations);

    for (int i = 1; i < iterations; i++)
    {
        *remAddr |= (uint8_t) rem >> ((64 - (i * 8)));
        *remAddr |= 0xff;
        remAddr += 1;
    }
    */
}


void CountingQF::printCQFrems()
{
    uint8_t * blockAddr = qf;
    for (uint64_t i = 0; i < numberOfBlocks; i++)
    {
        printf("\n\nBlock %lu:\n", i);
        printf("Offset:\n");
        printbits(*blockAddr, 8);

        uint8_t * occ = blockAddr + 1;
        uint8_t * run = blockAddr + 9;

        printf("\nOccupieds:\n");
        printbits64(*((uint64_t *)occ));
        printf("\nRunends:\n");
        printbits64(*((uint64_t *)run));
        
        for (uint64_t slot = 0; slot < numberOfSlots; slot++)
        {
            int pos = remainderPos[slot][0];
            int shiftBy = remainderPos[slot][1];
            int shiftByNext = remainderPos[((slot + 1) % 64)][1];

            uint8_t * remAddr = blockAddr + 17 + pos;

    
            uint8_t * rem = remAddr;
            *rem <<= shiftBy;
            uint64_t remPrint = (*((uint64_t *)rem) >> shiftBy);
            uint64_t mask = (1ULL << (remainderLen - shiftByNext)) - 1;
            
            printf("\nRem %lu:\n", slot);
            printbits(remPrint & mask, 58);
        }
        blockAddr += blockByteSize;
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
