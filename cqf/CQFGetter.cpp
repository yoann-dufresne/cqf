#include <CQFGetter.hpp>

#define MEM_UNIT 8
#define MAX_UINT 64

CQFGetter::CQFGetter(CountingQF cqf)
{
    this -> cqf = cqf;
    this -> block_ptr = cqf -> qf;
    this -> mem_unit_ptr = cqf -> qf + 17;
    this -> curr_slot_abs = 0ULL;
    this -> curr_slot_rel = 0ULL;
    this -> curr_val = 0ULL;
}

uint64_t CQFGetter::getCurrentValue() {
    curr_val = cqf.get_rem_block(block_ptr, curr_slot_rel);
    curr_val |= (curr_slot_abs << cqf.remainder_len);
    return (curr_val);
}

void CQFGetter::next() {
    uint64_t bits_left = cqf.remainder_len;
    uint64_t first_bit_offset = (curr_slot_rel * cqf -> remainder_len) % MEM_UNIT   
    
    curr_slot_rel += 1;
    curr_slot_abs += 1;
    bits_left -= first_bit_offset;
    mem_unit_ptr += 1;
    
    while (bits_left > 8) {          
        mem_unit_ptr += 1;
        bits_left -= MEM_UNIT;
    }
    
    if (curr_slot_rel > 63) {
        curr_slot_rel %= MAX_UINT;
        mem_unit_ptr += 17;
        block_ptr += cqf.block_byte_size;
    } 
}
