#ifndef CQFGETTER_HPP
#define CQFGETTER_HPP

#include <CountingQF.hpp>

class CQFGetter {
    public:
        CountingQF &cqf;
        uint64_t curr_slot_abs;
        uint64_t curr_slot_rel;
        uint64_t next_run_in_block;

        uint8_t * curr_block_ptr;
        bool in_run;

        explicit CQFGetter(CountingQF& cqf);

        uint64_t get_current_value();
        void next();

        void find_next_run();
        void next_run_val();
};
#endif