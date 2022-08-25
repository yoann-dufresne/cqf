#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <bitset>

#define MEM_UNIT 8
#define MAX_UINT 64
using namespace std;

CountingQF::CountingQF(uint32_t power_of_two)
{
    uint64_t number_of_slots = 1ULL << power_of_two;
 
    uint64_t total_occ_len = number_of_slots;
    uint64_t total_run_len = number_of_slots;

    uint64_t quotient_len = power_of_two;
    uint64_t rem_len = MAX_UINT - quotient_len;

    uint64_t total_rems_len = number_of_slots * rem_len;

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
    uint64_t slot = (val >> remainder_len);
    uint64_t rem = (val & ((1ULL << remainder_len) - 1));

    uint8_t * block_start = qf + block_byte_size * (slot / MAX_UINT);
    uint8_t * occupieds = block_start + 1;
    uint8_t * runends = block_start + 9;

    uint64_t run_start_slot = find_run_start(block_start, slot);

    uint64_t new_run_rank = asm_rank((*(uint64_t *)occupieds), run_start_slot);
    uint64_t runend_slot = asm_select((*(uint64_t *)runends), new_run_rank - 1);

    // Means run hasn't been created yet.
    // Simply set runend, occupied, and remainder.
    // Then return.
    if (new_run_rank == 0) {
        if (get_nth_bit_from((*(uint64_t *)occupieds), slot % MAX_UINT) == 0)
            set_nth_bit_from((*(uint64_t *)occupieds), slot % MAX_UINT);

        if (get_nth_bit_from((*(uint64_t *)runends), slot % MAX_UINT) == 0)
            set_nth_bit_from((*(uint64_t *)runends), slot % MAX_UINT);
        
        set_rem(slot, val);

        return;
    }

    // Means run is potentially multiblock. For now, just exit with error message
    if (runend_slot == MAX_UINT) {
        cerr << "Multiblock run" << endl;
        exit(0xb000dead);
    }

    uint64_t insertion_slot;
    insertion_slot = find_insert_slot(block_start, run_start_slot, rem);


    uint64_t occ_rank = asm_rank((*(uint64_t *)occupieds), insertion_slot);
    uint64_t run_sel = asm_select((*(uint64_t *)runends), occ_rank - 1);

    if (get_nth_bit_from((*(uint64_t *)occupieds), slot % MAX_UINT) == 0) {
        set_nth_bit_from((*(uint64_t *)occupieds), slot % MAX_UINT);
        set_nth_bit_from((*(uint64_t *)runends), run_sel + 1);
    }
    else {
        set_nth_bit_from((*(uint64_t *)runends), run_sel + 1);
        clear_nth_bit_from((*(uint64_t *)runends), run_sel);
    }

    shift_right_from(block_start, insertion_slot);
    set_rem(insertion_slot, val);

}

void CountingQF::shift_right_from(uint8_t * block_start, uint64_t insertion_slot)
{
    uint8_t * occupieds = block_start + 1;
    uint8_t * runends = block_start + 9;

    uint64_t rel_slot = insertion_slot % MAX_UINT;

    uint64_t occ_rank = asm_rank((*(uint64_t *)occupieds), rel_slot);
    uint64_t run_sel = asm_select((*(uint64_t *)runends), occ_rank - 1);

    uint64_t curr_slot = run_sel;
    
    while (curr_slot >= rel_slot && curr_slot != MAX_UINT) {
        set_rem_block(block_start, curr_slot + 1 , get_rem_block(block_start, curr_slot));
        curr_slot -= 1;
    }
}

uint64_t CountingQF::find_insert_slot(uint8_t * block_start, uint64_t run_start_slot, uint64_t rem)
{
    uint8_t * runends = block_start + 9;
    
    uint64_t rel_slot = run_start_slot % MAX_UINT;

    while (get_rem(run_start_slot) < rem
        &&  get_nth_bit_from(*((uint64_t *) runends), rel_slot) == 0
        &&  run_start_slot < number_of_slots)
    {
        rel_slot++;
        run_start_slot++;
    }

    if (get_nth_bit_from(*((uint64_t *) runends), rel_slot) && rem > get_rem(run_start_slot))
        return (run_start_slot + 1);
    
    return (run_start_slot);
}

uint64_t CountingQF::find_run_start(uint8_t * block_start, uint64_t slot)
{
    uint8_t * occupieds = block_start + 1;
    uint8_t * runends = block_start + 9;

    uint64_t rel_slot = slot % MAX_UINT;
    uint64_t block_start_slot = slot - rel_slot;
    
    uint64_t occ_rank = asm_rank((*(uint64_t *)occupieds), rel_slot - 1);

    if (occ_rank == 0)
        return slot;

    uint64_t run_sel = asm_select((*(uint64_t *)runends), occ_rank - 1);

    return (block_start_slot + run_sel + 1);
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