#include <CQFGetter.hpp>
#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <bitset>

#define MEM_UNIT 8
#define MAX_UINT 64
/*
=====================================================================================
Hitting out of bounds memory on the last remainder, works fine until it doesn't.
=====================================================================================
*/

CQFGetter::CQFGetter(CountingQF& cqf): cqf(cqf)
{
    this -> cqf = cqf;
    this -> curr_block_ptr = cqf.qf;
    this -> curr_slot_abs = 0ULL;
    this -> curr_slot_rel = 0ULL;
    this -> next_run_in_block = 0ULL;
    this -> block_no = 0ULL;
    this -> in_run = false;
    next();
}

uint64_t CQFGetter::get_current_value() {
    uint64_t curr_val = 0;
    
    curr_val = cqf.get_rem(curr_slot_abs);
    curr_val += (curr_slot_abs << cqf.remainder_len);

    return (curr_val);
}


void CQFGetter::next() {
    if (has_next()) {
        if (!in_run)
            find_next_run();
        else
            next_run_val();
    }                                                             
}

bool CQFGetter::has_next() {
    uint64_t abs_copy = curr_slot_abs;
    uint64_t rel_copy = curr_slot_rel;
    uint64_t run_copy = next_run_in_block;
    uint8_t * blk_ptr_copy = curr_block_ptr;

    while (abs_copy < cqf.number_of_slots) {
        uint64_t * occupieds = ((uint64_t *) (blk_ptr_copy + 1));

         while (*occupieds == 0) {
            blk_ptr_copy += cqf.block_byte_size;
            block_no += 1;

            if (block_no >= cqf.number_of_blocks)
                return false;

            occupieds = ((uint64_t *) (blk_ptr_copy + 1));
            rel_copy = 0;
            run_copy = 0;
            abs_copy += MAX_UINT;
        }

        uint64_t old_slot_rel = rel_copy;

        if (abs_copy >= cqf.number_of_slots)
            return false;

        rel_copy = asm_select(*occupieds, run_copy);
        run_copy += 1;
        
        abs_copy += rel_copy - old_slot_rel;

        if (abs_copy >= cqf.number_of_slots)
            return false;

        if (rel_copy == MAX_UINT) {
            blk_ptr_copy += cqf.block_byte_size;
            block_no += 1;
            
            run_copy = 0;
            rel_copy = 0;
        }
        else
            return true;
    }

    return false;
}

void CQFGetter::find_next_run() {
    while (!in_run)
    {
        uint64_t * occupieds = ((uint64_t *) (curr_block_ptr + 1));

        // Skip over empty blocks
        while (*occupieds == 0) {
            curr_block_ptr += cqf.block_byte_size;
            occupieds = ((uint64_t *) (curr_block_ptr + 1));
            curr_slot_rel = 0;
            next_run_in_block = 0;
            curr_slot_abs += MAX_UINT;
        }

        uint64_t old_slot_rel = curr_slot_rel;

        // Get start of our next run
        curr_slot_rel = asm_select(*occupieds, next_run_in_block);
        next_run_in_block += 1;
        
        curr_slot_abs += curr_slot_rel - old_slot_rel;

        
        // If the start of our next run isn't present in this block
        // Go to the next one and repeat steps
        if (curr_slot_rel == MAX_UINT) {
            curr_block_ptr += cqf.block_byte_size;
            next_run_in_block = 0;
            curr_slot_rel = 0;
        }
        else {
            in_run = true;
        }
    }              
}


void CQFGetter::next_run_val() {
    uint64_t * runends = ((uint64_t *) (curr_block_ptr + 9));

    //If the value we're on is a runend, go find the next run
    if (get_nth_bit_from(*runends, curr_slot_rel) == 1) {
        in_run = false;
        next();
    }
    else {
        // Go forward one slot
        curr_slot_rel += 1;
    
        // If the next value isn't in this block, go to the next one
        if (curr_slot_rel == MAX_UINT) {
            curr_block_ptr += cqf.block_byte_size;
            curr_slot_rel = 0;
            next_run_in_block = 0;
        }
    }
}