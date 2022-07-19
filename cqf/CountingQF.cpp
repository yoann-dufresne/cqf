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

uint64_t CountingQF::get_rem(uint32_t slot)
{
    uint8_t * block_start = qf + block_byte_size * (slot / MAX_UINT);

    uint64_t res = 0;
    
    uint32_t first_slot_offset = (slot * remainder_len) / MEM_UNIT;
    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT;
    uint32_t last_bit_offset = ((slot + 1) * remainder_len) % MEM_UNIT;
    uint64_t quotient_mask = ((1ULL << remainder_len) - 1);
    uint64_t bits_left = remainder_len;

    uint8_t * slot_addr = block_start + 17 + first_slot_offset;  

    res += *slot_addr;
    cout << "Getting: " << bitset<8>(*slot_addr) << endl;
    bits_left -= (MEM_UNIT - bit_offset);

    while (bits_left > 8)
    {
        slot_addr += 1;
        
        res <<= MEM_UNIT;
        res += *slot_addr;
        cout << "Getting: " << bitset<8>(*slot_addr) << endl;
        
        bits_left -= 8;
    }

    slot_addr += 1;
    
    res <<= MEM_UNIT;
    res += *slot_addr;
    
    cout << "Getting: " << bitset<8>(*slot_addr) << endl;
    cout << endl;

    return ((res >> ((MEM_UNIT - last_bit_offset) % MEM_UNIT)) & quotient_mask);
}

using namespace std;
void CountingQF::set_rem(uint32_t slot, uint64_t value)
{
    uint8_t * block_start = qf + block_byte_size * (slot / MAX_UINT);

    uint32_t bit_offset = (slot * remainder_len) % MEM_UNIT;
    
    uint64_t last_slot_offset = ((slot + 1) * remainder_len) / MEM_UNIT;
    uint32_t last_bit_offset =  ((slot + 1) * remainder_len) % MEM_UNIT;

    uint64_t quotient_mask = ((1ULL << remainder_len) - 1);
    uint64_t bits_left = remainder_len;

    uint64_t rem = value & quotient_mask;

    rem <<= (MEM_UNIT - last_bit_offset) % MEM_UNIT;

    uint8_t first_byte_mask = ~((1ULL << ((MEM_UNIT - last_bit_offset) % MEM_UNIT)) - 1);
    uint8_t last_byte_mask = ((1ULL << (MEM_UNIT - bit_offset)) - 1);
    
    uint8_t * slot_addr = block_start + 17 + last_slot_offset;

    cout << bitset<64>(rem) << endl;
    set8(slot_addr, rem & 0xff, first_byte_mask);
    cout << "Rem: " << bitset<64>(rem) << endl;
    cout << "Setting: " << bitset<8>(*slot_addr) << " with mask: " << bitset<8>(first_byte_mask) << endl;
    bits_left -= last_bit_offset;

    while (bits_left > 8)
    {   
        slot_addr -= 1;

        rem >>= MEM_UNIT;
        set8(slot_addr, rem & 0xff, 0xff);
        cout << "Rem: " << bitset<64>(rem) << endl;
        cout << "Setting: " << bitset<8>(*slot_addr) << endl;

        bits_left -= 8;
    }

    slot_addr -= 1;
    rem >>= MEM_UNIT;
    // Set the last part of the remainder 
    set8(slot_addr, (rem) & 0xff, last_byte_mask);
    cout << "Rem: " << bitset<64>(rem) << endl;
    cout << "Setting: " << bitset<8>(*slot_addr) << " with mask: " << bitset<8>(last_byte_mask) <<  endl;
    cout << endl;
}

CountingQF::~CountingQF() {
    delete[] qf;
}


