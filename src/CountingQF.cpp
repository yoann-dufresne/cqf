#include <CountingQF.hpp>

//TODO: add macros

#define DEFAULT_VEC_LEN 64
#define ELEMENT_LEN 64
#define DEFAULT_SLOT_LEN 8

CountingQF::CountingQF()
{
    remainders[DEFAULT_VEC_LEN] = {};
    this -> occupieds = 0ULL;
    this -> runends = 0ULL;
}

CountingQF::CountingQF(uint64_t remaindersLen)
{
    //TODO: Resize remainders according to hash values in the future
    remainders[remaindersLen] = {};
    this -> occupieds = 0ULL;
    this -> runends = 0ULL;
}

bool CountingQF::query(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - DEFAULT_SLOT_LEN);
    
    if ((occupieds >> ELEMENT_LEN - slotPos) & 1)
        return false;
    
    int rem = (val << DEFAULT_SLOT_LEN) >> DEFAULT_SLOT_LEN;
    int occSlotsToPos = asmRank(occupieds, slotPos);
    int lastSlotInRun = asmSelect(runends, occSlotsToPos);

    while (lastSlotInRun > slotPos || bitAt(runends, lastSlotInRun) != 1)
    {
        if (remainders[lastSlotInRun] == rem)
            return true;
        
        lastSlotInRun--;
    }

    return false;
}

void CountingQF::insertValue(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - DEFAULT_SLOT_LEN);
    int rem = (val << DEFAULT_SLOT_LEN) >> DEFAULT_SLOT_LEN;

    int occSlotsToPos = asmRank(occupieds, slotPos);
    int lastSlotInRun = asmSelect(runends, occSlotsToPos);
    
    if (slotPos > lastSlotInRun)
    {
        remainders[slotPos] = rem;
        runends &= 1ULL << (DEFAULT_VEC_LEN - slotPos);
    }
    else 
    {
        lastSlotInRun++;
        int freeSlot = findFirstUnusedSlot(lastSlotInRun);

        // Travel back to end of run if necessary
        // TODO: show case where backwards traversal necessary
        while (freeSlot > lastSlotInRun) {
            remainders[freeSlot] = remainders[freeSlot - 1];
            // set the bit at freeSlot to the one at freeSlot - 1
            runends ^= (-bitAt(runends, freeSlot - 1) ^ runends) & (1ULL << freeSlot);
            freeSlot--;
        }

        if (bitAt(occupieds, slotPos) == 1) {
            runends &= ~(1ULL << (lastSlotInRun - 1));
        }

        runends |= 1ULL << lastSlotInRun;
        occupieds |= 1ULL << slotPos;
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
    val = val & ((2ULL) << pos) - 1;

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

uint8_t CountingQF::bitAt(uint64_t x, int pos)
{
    return (x >> (DEFAULT_VEC_LEN - pos) & 0b1);
}