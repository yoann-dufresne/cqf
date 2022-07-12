#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <bitset>

#define MEM_UNIT 8
#define MAX_UINT 64
using namespace std;

CountingQF::CountingQF(uint32_t power_of_two)
{
    /*
    * Need total remainders (h1) memory to be (x * (VEC_LEN - log2(x)))
    * aka. so len(h0) needs to be of len log2(x) bits to have enough
    * values for the possible slots.
    */

    uint64_t number_of_slots = 1ULL << power_of_two;
 
    uint64_t total_occ_len = number_of_slots;
    uint64_t total_run_len = number_of_slots;

    uint64_t quotient_len = power_of_two;
    uint64_t rem_len = MAX_UINT - quotient_len;

    uint64_t total_rems_len = number_of_slots * rem_len;

    /* Blocks are made of an offset (8b) + occ (64b) 
    + run (64b) + remainders (64b * rem_len) */

    uint64_t number_of_blocks = number_of_slots / MAX_UINT;
    number_of_blocks = (number_of_blocks == 0) ? number_of_blocks+1 : number_of_blocks;

    uint64_t filter_size = (8 * number_of_blocks) + total_occ_len + total_run_len + total_rems_len;
    
    uint64_t block_byte_size = filter_size / number_of_blocks / MEM_UNIT;

    this -> qf = new uint8_t[(block_byte_size * number_of_blocks)];

    memset(qf, 0, block_byte_size * number_of_blocks);

    this -> filter_size = filter_size;
    this -> number_of_slots = number_of_slots;
    this -> number_of_blocks = number_of_blocks;
    this -> block_byte_size = block_byte_size;
    this -> quotient_len = quotient_len;
    this -> remainder_len = rem_len;
}

bool CountingQF::query(uint64_t val)
{
    return false;
}

void CountingQF::insert_value(uint64_t val)
{
}

// Getting every byte from every uint8 which has a part of the remainder
// then right shifting and masking to keep only relevant bits.

uint64_t CountingQF::get_rem_rev(uint8_t * block_start, uint32_t slot)
{
    uint64_t res = 0;
    
    uint32_t slot_offset = (slot * remainder_len) / MEM_UNIT;
    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT;
    uint64_t quotient_mask = ((1ULL << remainder_len) - 1);

    uint32_t iterations = ((remainder_len + (MEM_UNIT - 1)) / MEM_UNIT);

    uint8_t * slot_addr = block_start + 17 + slot_offset + iterations;    

    res += *slot_addr;
    if (bit_offset % 2 != 0)
        res >>= 1;
    

    for (uint8_t i = 0; i < iterations; i++)
    {
        slot_addr -= 1;
        res <<= MEM_UNIT;
        res += *slot_addr;
    }
    
    return ((res >> bit_offset) & quotient_mask);
}

using namespace std;
void CountingQF::set_rem_rev(uint8_t * block_start, uint32_t slot, uint64_t value)
{
    uint32_t slot_offset = (slot * remainder_len) / MEM_UNIT; //cases a sauter
    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT; // bits a sauter

    cout << "bit_offset: " << bit_offset << endl;

    uint64_t quotient_mask = ((1ULL << remainder_len) - 1); //quotient mask
    uint64_t rem = value & quotient_mask;
    rem <<= bit_offset;

    uint8_t first_byte_mask = ~((1ULL << bit_offset) - 1);
    
    // block_start + Offset + Occupieds and Runends length + slot_offset
    uint8_t * slot_addr = block_start + 1 + ((MAX_UINT / MEM_UNIT) * 2) 
        + slot_offset;
    // Set the first part of the remainder
    set8(slot_addr, rem & 0xff, first_byte_mask);


    // How many parts are left in the "middle" memory units
    uint32_t iterations = ((remainder_len + (MEM_UNIT - 1)) / MEM_UNIT);
    for (uint8_t i = 1; i < iterations; i++)
    {
        slot_addr += 1;
                    // Get bits corresponding to given part of the remainder 
        set8(slot_addr, (rem >> (MEM_UNIT * i)) & 0xff, 0xff);
    }

    slot_addr += 1;
    uint32_t remaining_bits = ((slot + 1) * remainder_len) % MEM_UNIT;

    // Mask to keep only rightmost bits of last memory unit.
    uint8_t last_byte_mask = ((1ULL << remaining_bits) - 1);
    cout << "remaining_bits: " << remaining_bits << endl;
    // Set the last part of the remainder 
    set8(slot_addr, (rem >> (remainder_len - remaining_bits)) & 0xff, last_byte_mask);
}

CountingQF::~CountingQF() {
    delete[] qf;
}