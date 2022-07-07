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

uint64_t CountingQF::get_rem_rev(uint8_t * block_start, uint32_t slot)
{
    uint64_t res = 0;
    int get_calls = 0;
    uint32_t slot_offset = (slot * remainder_len) / MEM_UNIT;
    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT;
    uint32_t iterations = ((remainder_len + (MEM_UNIT - 1)) / MEM_UNIT) - 2;
    
    uint8_t remaining_bits = remainder_len - ((MEM_UNIT * (iterations + 1)) + (MEM_UNIT - bit_offset));
    uint8_t first_byte_mask = ((1ULL << (MEM_UNIT - remaining_bits)) - 1);

    uint8_t * slot_addr = block_start + 17 + slot_offset + iterations + 2;    

    cout << "get first byte mask" <<  bitset<8>(first_byte_mask)<< endl;
    
    res += (*slot_addr & first_byte_mask);
    get_calls++;

    for (uint8_t i = 0; i <= iterations; i++)
    {
        slot_addr -= 1;
        res <<= MEM_UNIT;
        res += *slot_addr; 
        get_calls++;
    }

    slot_addr -= 1;

    res <<= MEM_UNIT;
    uint8_t last_byte_mask = ~((1ULL << bit_offset) - 1);
    
    cout << "get last byte mask" << bitset<8>(last_byte_mask) << endl;   

    res += (*slot_addr & last_byte_mask);
    get_calls++;
    cout << "get calls:" << get_calls << endl;
    return (res);
}

using namespace std;
void CountingQF::set_rem_rev(uint8_t * block_start, uint32_t slot, uint64_t rem)
{
    uint32_t slot_offset = (slot * remainder_len) / MEM_UNIT;
    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT;

    uint8_t first_byte_mask = ~((1ULL << bit_offset) - 1);
    cout << "set first byte mask" <<  bitset<8>(first_byte_mask) << endl;

    int set_calls = 0;
    // block_start + Offset + Occupieds and Runends length + slot_offset
    uint8_t * slot_addr = block_start + 1 + ((MAX_UINT / MEM_UNIT) * 2) 
        + slot_offset;

    // Set the first part of the remainder
    set8(slot_addr, rem, first_byte_mask);
    set_calls++;

    // How many parts are left in memory units
    uint32_t iterations = ((remainder_len + (MEM_UNIT - 1)) / MEM_UNIT) - 2;

    for (uint8_t i = 0; i <= iterations; i++)
    {
        slot_addr += 1;
                    // Get bits corresponding to given part of the remainder 
        set8(slot_addr,(rem >> (MEM_UNIT * i)) & 0xff,0xff);  
        set_calls++;
    }

    slot_addr += 1;

    uint32_t remaining_bits = remainder_len - ((MEM_UNIT * (iterations + 1)) + (MEM_UNIT - bit_offset));

    // Mask to keep only rightmost bits of last memory unit.
    uint8_t last_byte_mask = ((1ULL << (MEM_UNIT - remaining_bits)) - 1);
    cout << "set last byte mask" <<  bitset<8>(last_byte_mask)<< endl;   
    // Set the last part of the remainder   
    set8(slot_addr, (rem >> (MAX_UINT - MEM_UNIT)) & 0xff, last_byte_mask);
    set_calls++;
    cout << "set calls: " << set_calls << endl;
}

CountingQF::~CountingQF() {
    delete[] qf;
}