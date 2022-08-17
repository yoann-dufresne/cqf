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

uint64_t find_first_unused_slot(uint8_t * block_start, uint64_t free_slot) {
    uint8_t * occupieds = block_start + 1;
    uint8_t * runends = block_start + 9;

    uint64_t occ_slots_to_pos = asm_rank(*occupieds, free_slot);
    uint64_t next_runend = asm_select(*runends, occ_slots_to_pos);

    // While the free slot is not free (is in a run)
    while (free_slot <= next_runend) {
        free_slot = next_runend + 1;
        // Update the next run to see if there is another run immediately after
        // our runend (so not a free slot)
        // TODO: multiblock
        occ_slots_to_pos = asm_rank(*occupieds, free_slot);
        next_runend = asm_select(*runends, occ_slots_to_pos);
    }

    return (free_slot);
}

void CountingQF::insert_value(uint64_t val)
{
    uint64_t slot = (val >> remainder_len);
    uint64_t rem = val & ((1ULL << remainder_len) - 1);

    uint64_t rel_slot = slot % MAX_UINT;
    uint64_t block_start_slot = slot - rel_slot;
    uint8_t * block_start = qf + (block_byte_size * (slot / MAX_UINT));
    uint8_t * occupieds = block_start + 1;
    uint8_t * runends = block_start + 9;

    uint64_t zeros_to_slot = asm_rank((*(uint64_t *)occupieds), rel_slot);
    uint64_t runend_slot = asm_select((*(uint64_t *)runends), zeros_to_slot - 1);

    // cout << endl;
    // cout << "Inserting: " << rem << " at " << rel_slot << endl;
    // cout << "occ " << bitset<64>((*(uint64_t *)occupieds)) << endl;
    // cout << "run " << bitset<64>((*(uint64_t *)runends)) << endl;

    // Single insertion collision
    if (get_nth_bit_from((*(uint64_t *)occupieds), rel_slot) == 1 && get_nth_bit_from((*(uint64_t *)runends), rel_slot) == 1) {

        clear_nth_bit_from((*(uint64_t *)runends), slot % MAX_UINT);
        set_nth_bit_from((*(uint64_t *)runends), (slot + 1) % MAX_UINT);

        // Sort the remainders in the slot by increasing order
        if (get_rem(slot) > rem) {
            set_rem(slot + 1, get_rem(slot));
            set_rem(slot, val);
        }
        else
            set_rem(slot + 1, val);

        return;
    }

    // Run collision
    if (zeros_to_slot != 0 && runend_slot >= rel_slot &&
        get_nth_bit_from((*(uint64_t *)occupieds), rel_slot) == 1) {
        uint64_t sorted_rem_slot = runend_slot;


        // Find sorted rem placement
        // TODO: Turn this into a dichotomic search instead of linear
        // TODO: Multiblock search
        while (get_rem((block_start_slot + sorted_rem_slot)) >= rem && get_nth_bit_from((*(uint64_t *)occupieds), sorted_rem_slot) != 1) {
            sorted_rem_slot -= 1;
        }

        uint64_t shift_end_slot = (runend_slot + 1) % MAX_UINT;
        set_nth_bit_from((*(uint64_t *)runends), runend_slot + 1);
        clear_nth_bit_from((*(uint64_t *)runends), runend_slot);

        // If the remainder should be at runend
        if (sorted_rem_slot > runend_slot) {
            set_rem(block_start_slot + sorted_rem_slot, rem);
        }
        else {
            // Move everything from sorted rem one slot to the right
            // TODO: check collisions with next run
            while (shift_end_slot > sorted_rem_slot) {
                set_rem(block_start_slot + shift_end_slot, get_rem(block_start_slot + shift_end_slot - 1));
                shift_end_slot -= 1;
            }

            set_rem(block_start_slot + sorted_rem_slot, rem);
        }

    }
    // No collision but inside run
    else if (get_nth_bit_from((*(uint64_t *)occupieds), rel_slot) == 1 && rel_slot <= runend_slot) {
        set_nth_bit_from((*(uint64_t *)occupieds), rel_slot);
        set_nth_bit_from((*(uint64_t *)runends), runend_slot + 1);
        set_rem(runend_slot + 1, val);
    }
    else {
        set_rem(slot, val);
        set_nth_bit_from((*(uint64_t *)occupieds), rel_slot);
        set_nth_bit_from((*(uint64_t *)runends), rel_slot);
    }

//     cout << endl;
//     cout << "occ " << bitset<64>((*(uint64_t *)occupieds)) << endl;
//     cout << "run " << bitset<64>((*(uint64_t *)runends)) << endl;
}

void CountingQF::set_rem(uint32_t slot, uint64_t value)
{
    uint8_t * block_start = qf + block_byte_size * (slot / MAX_UINT);

    uint64_t block_slot = slot % MAX_UINT;
    uint64_t quotient_mask = ((1ULL << remainder_len) - 1);
    uint64_t rem = value & quotient_mask;

    set_rem_block(block_start, block_slot, rem);
}

uint64_t CountingQF::get_rem(uint32_t slot)
{
    uint8_t * block_start = qf + block_byte_size * (slot / MAX_UINT);
    uint64_t block_slot = slot % MAX_UINT;

    return (get_rem_block(block_start, block_slot));
}

void CountingQF::set_rem_block(uint8_t * block_start, uint64_t block_slot, uint64_t rem)
{
    uint64_t first_slot_byte = (block_slot * remainder_len) / MEM_UNIT;
    uint64_t first_bit_offset = (block_slot * remainder_len) % MEM_UNIT;

    uint64_t first_bit = (first_slot_byte * MEM_UNIT) + first_bit_offset;

    uint64_t last_bit = first_bit + (remainder_len - 1);
    uint64_t last_slot_byte = last_bit / MEM_UNIT;

    uint64_t last_bit_offset =  ((block_slot + 1) * remainder_len) % MEM_UNIT;

    uint64_t bits_left = remainder_len;

    rem <<= (MEM_UNIT - last_bit_offset) % MEM_UNIT;

    uint8_t first_byte_mask = ~((1ULL << ((MEM_UNIT - last_bit_offset) % MEM_UNIT)) - 1);
    uint8_t last_byte_mask = ((1ULL << (MEM_UNIT - first_bit_offset)) - 1);
    
    uint8_t * slot_addr = block_start + 17 + last_slot_byte;

    set8(slot_addr, rem & 0xff, first_byte_mask);
    bits_left -= (last_bit_offset == 0) ? MEM_UNIT : last_bit_offset;

    while (bits_left > 8)
    {   
        slot_addr -= 1;

        rem >>= MEM_UNIT;
        set8(slot_addr, rem, 0xff);
        bits_left -= 8;
    }

    slot_addr -= 1;
    rem >>= MEM_UNIT;
    set8(slot_addr, (rem) & 0xff, last_byte_mask);
}

uint64_t CountingQF::get_rem_block(uint8_t * block_start, uint64_t block_slot)
{
    uint64_t res = 0;

    uint64_t quotient_mask = ((1ULL << remainder_len) - 1);
    
    uint64_t first_slot_byte = (block_slot * remainder_len) / MEM_UNIT;
    
    uint64_t first_bit_offset = (block_slot * remainder_len) % MEM_UNIT;
    uint64_t last_bit_offset = ((block_slot + 1) * remainder_len) % MEM_UNIT;
    
    uint64_t bits_left = remainder_len;

    uint8_t * slot_addr = block_start + 17 + first_slot_byte;  

    res += *slot_addr;

    bits_left -= (MEM_UNIT - first_bit_offset);

    while (bits_left > 8)
    {
        slot_addr += 1;
        
        res <<= MEM_UNIT;
        res += *slot_addr;
        
        bits_left -= 8;
    }

    slot_addr += 1;
    
    res <<= MEM_UNIT;
    res += *slot_addr;

    return ((res >> ((MEM_UNIT - last_bit_offset) % MEM_UNIT)) & quotient_mask);
}

void CountingQF::reset() {
    memset(qf, 0, block_byte_size * number_of_blocks);
}

CountingQF::~CountingQF() {
    delete[] qf;
}


