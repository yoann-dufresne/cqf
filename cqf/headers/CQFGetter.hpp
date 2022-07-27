#ifndef CQFGETTER_HPP
#define CQFGETTER_HPP

#include <CountingQF.hpp>

class CQFGetter {
    public:
        CountingQF &cqf;
        uint64_t curr_slot_abs;

        explicit CQFGetter(CountingQF& cqf);

        uint64_t get_current_value();
        void next();
};
#endif