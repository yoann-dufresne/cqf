#ifndef CQFGETTER_HPP
#define CQFGETTER_HPP

#include <CountingQF.hpp>

class CQFGetter {
    public:
        CountingQF cqf;
        uint8_t * block_ptr;
        uint8_t * mem_unit_ptr;

        uint64_t curr_slot_rel;
        uint64_t curr_slot_abs;
        uint64_t curr_val;    

        explicit CQFGetter(CountingQF cqf);

        uint64_t getCurrentValue();
        void next();
};
#endif