#ifndef CQFGETTER_HPP
#define CQFGETTER_HPP

#include <CountingQF.hpp>

class CQFGetter {
    public:
        CountingQF &cqf;
        uint64_t curr_slot_abs;
        uint64_t curr_slot_rel;
        uint64_t next_run_in_block;
        uint64_t block_no;

        uint8_t * curr_block_ptr;
        bool in_run;

        explicit CQFGetter(CountingQF& cqf);

        /**
         * @brief Checks if there is a next value inserted into the CQF.
         * 
         * @return true 
         * @return false 
         */
        bool has_next();

        /**
         * @brief Moves current value pointer to the next value.
         * 
         */
        void next();

        /**
         * @brief Reconstructs the current value from the information given.
         * 
         * @return uint64_t Reconstructed val.
         */
        uint64_t get_current_value();

        /**
         * @brief Finds the next run of remainders.
         * 
         */
        void find_next_run();

        /**
         * @brief Finds the next value in a run.
         * 
         */
        void next_run_val();
};
#endif