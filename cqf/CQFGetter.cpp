#include <CQFGetter.hpp>
#include <CountingQF.hpp>
#include <bitset>

#define MEM_UNIT 8
#define MAX_UINT 64

CQFGetter::CQFGetter(CountingQF& cqf): cqf(cqf)
{
    this -> cqf = cqf;
    this -> curr_slot_abs = 0ULL;
}

uint64_t CQFGetter::get_current_value() {
    uint64_t curr_val = 0;
    
    curr_val = cqf.get_rem(curr_slot_abs);
    curr_val += (curr_slot_abs << cqf.remainder_len);

    return (curr_val);
}

void CQFGetter::next() {
    curr_slot_abs += 1;
}
