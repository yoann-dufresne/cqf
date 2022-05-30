#include <CountingQF.hpp>

#define VEC_LEN 64
#define ELEMENT_LEN 64
#define SLOT_LEN 8

CountingQF::CountingQF()
{
    this -> remainders = std::vector<uint64_t>(VEC_LEN, 0);
    this -> occupieds = std::bitset<VEC_LEN>{0x00};
    this -> runends = std::bitset<VEC_LEN>{0x00};
}

bool CountingQF::query(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - SLOT_LEN);

    if (!occupieds[slotPos])
        return false;

    int rem = (val << SLOT_LEN) >> SLOT_LEN;
    int occSlotsToPos = asmRank(occupieds.to_ulong(), slotPos);
    int lastSlotInRun = asmSelect(runends.to_ulong(), occSlotsToPos);

    while (lastSlotInRun > slotPos || runends[lastSlotInRun] != 1) 
    {
        if (remainders[lastSlotInRun] == rem)
            return true;
        
        lastSlotInRun -= 1;
    }

    return false;
}

void CountingQF::insertValue(uint64_t val)
{
    int slotPos = val >> (ELEMENT_LEN - SLOT_LEN);
    int rem = (val << SLOT_LEN) >> SLOT_LEN;

    int occSlotsToPos = asmRank(occupieds.to_ulong(), slotPos);
    int lastSlotInRun = asmSelect(runends.to_ulong(), occSlotsToPos);

    if (slotPos > lastSlotInRun)
    {
        remainders[slotPos] = rem;
        runends[slotPos] = 1;
    }
    else
    {
        lastSlotInRun += 1;
        int freeSlot = findFirstUnusedSlot(lastSlotInRun);

        while (freeSlot > lastSlotInRun)
        {
            remainders[freeSlot] = remainders[freeSlot - 1];
            runends[freeSlot] = runends[freeSlot - 1];
            freeSlot -= 1;
        }

        remainders[lastSlotInRun] = rem;
        
        if (occupieds[slotPos])
            runends[lastSlotInRun - 1] = 0;
        
        runends[lastSlotInRun] = 1;
    }
    occupieds[slotPos] = 1;
}

int CountingQF::findFirstUnusedSlot(int fromPos)
{
    int occSlotsToPos = asmRank(occupieds.to_ulong(), fromPos);
    int lastSlotInRun = asmSelect(runends.to_ulong(), occSlotsToPos);

    while (fromPos <= lastSlotInRun)
    {
        fromPos = lastSlotInRun + 1;
        occSlotsToPos = asmRank(occupieds.to_ulong(), fromPos);
        lastSlotInRun = asmSelect(runends.to_ulong(), occSlotsToPos);
    }

    return fromPos;
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
