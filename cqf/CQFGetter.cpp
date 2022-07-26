#include <CQFGetter.hpp>
#include <CountingQF.hpp>

#define MEM_UNIT 8
#define MAX_UINT 64

CQFGetter::CQFGetter(CountingQF& cqf): cqf(cqf)
{
    this -> cqf = cqf;
    this -> block_ptr = cqf.qf;
    this -> mem_unit_ptr = cqf.qf + 17;
    this -> curr_slot_abs = 0ULL;
    this -> curr_slot_rel = 0ULL;
    this -> curr_val = 0ULL;
}

uint64_t CQFGetter::get_current_value() {
    curr_val = 0;
    curr_val = cqf.get_rem_block(block_ptr, curr_slot_rel);
    curr_val |= (curr_slot_abs << cqf.remainder_len);
    return (curr_val);
}

void CQFGetter::next() {
    uint64_t bits_left = cqf.remainder_len;
    uint64_t first_bit_offset = (curr_slot_rel * cqf.remainder_len) % MEM_UNIT;
    
    curr_slot_rel += 1;
    curr_slot_abs += 1;
    
    bits_left -= first_bit_offset;
    mem_unit_ptr += 1;
    
    while (bits_left > 8) {          
        bits_left -= MEM_UNIT;
        mem_unit_ptr += 1;
    }

    if (curr_slot_rel == MAX_UINT) {
        curr_slot_rel %= MAX_UINT;
        block_ptr += cqf.block_byte_size;
        mem_unit_ptr = block_ptr + 17;
    } 
}
