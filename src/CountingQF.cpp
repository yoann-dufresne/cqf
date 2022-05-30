#include <CountingQF.hpp>

#define VEC_LEN 64
#define ELEMENT_LEN 64
#define SLOT_LEN 8

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

CountingQF::CountingQF()
{
    remainders[VEC_LEN] = {};
    this -> occupieds = 0ULL;
    this -> runends = 0ULL;
}

bool CountingQF::query(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - SLOT_LEN);
    
    if (getNthBitFrom(occupieds, (ELEMENT_LEN - slotPos)) == 0)
        return false;
    
    uint64_t rem = (val << SLOT_LEN) >> SLOT_LEN;
    int occSlotsToPos = asmRank(occupieds, slotPos);
    int lastSlotInRun = asmSelect(runends, occSlotsToPos);

    while (lastSlotInRun >= slotPos 
        || getNthBitFrom(runends, lastSlotInRun) != 1)
    {
        if (remainders[lastSlotInRun] == rem)
            return true;
        
        lastSlotInRun--;
    }

    return false;
}

void CountingQF::insertValue(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - SLOT_LEN);
    uint64_t rem = (val << SLOT_LEN) >> SLOT_LEN;

    int occSlotsToPos = asmRank(occupieds, slotPos);
    int lastSlotInRun = asmSelect(runends, occSlotsToPos);
    
    if (slotPos > lastSlotInRun)
    {
        remainders[slotPos] = rem;
        setNthBitFrom(runends, VEC_LEN - slotPos);
    }
    else 
    {
        lastSlotInRun++;
        int freeSlot = findFirstUnusedSlot(lastSlotInRun);
        
        while (freeSlot > lastSlotInRun) {
            remainders[freeSlot] = remainders[freeSlot - 1];

            // set the bit at freeSlot to the one at freeSlot - 1
            setNthBitToX(runends, freeSlot,
                        getNthBitFrom(runends, freeSlot - 1));
            
            freeSlot--;
        }

        if (getNthBitFrom(occupieds, slotPos) == 1) {
            clearNthBitFrom(runends, lastSlotInRun - 1);
        }

        setNthBitFrom(runends, lastSlotInRun);
        setNthBitFrom(occupieds, slotPos);
    }
}

int CountingQF::findFirstUnusedSlot(int fromPos)
{
    int occSlotsToPos = asmRank(occupieds, fromPos);
    int lastSlotInRun = asmSelect(runends, occSlotsToPos);

    while (fromPos <= lastSlotInRun) {
        fromPos = lastSlotInRun + 1;
        occSlotsToPos = asmRank(occupieds, fromPos);
        lastSlotInRun = asmSelect(runends, occSlotsToPos);
    }

    return fromPos;
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

