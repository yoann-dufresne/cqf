#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#define MEM_UNIT 8
#define MAX_UINT 64

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
/**
 * @brief Counting Quotient Filter class
 * 
 */
class CountingQF 
{   
    public:
        uint64_t number_of_slots;
        uint64_t number_of_blocks;
        uint64_t block_bit_size;
        uint64_t block_byte_size;

        uint64_t quotient_len;
        uint64_t remainder_len;

        uint64_t filter_size;

        uint8_t * qf;

        explicit CountingQF(uint32_t n);

        ~CountingQF();
        
        /**
         * @brief Query CQF for presence.
         * 
         * @param val Check if given value is present in the CQF, 
         *            CQFs may announce false-positives.
         * @return true
         * @return false
         */
        bool query(uint64_t val);

        /**
         * @brief (Single block only) Insert a 64-bit value into the CQF.
         * 
         * @param val Value to insert.
         */
        void insert_value(uint64_t val);

         /**
         * @brief Set the remainder at the slot relative to block_start
         * to the value of rem.
         * 
         * @param block_start Block to set a value in.
         * @param slot Slot relative to the block.
         * @param rem remainder to set the value as.
         */
        void set_rem_block(uint8_t * block_start, uint64_t block_slot, uint64_t rem);

        /**
         * @brief Get the remainder at the slot relative to block_start.
         * 
         * @param block_start Point to said block.
         * @param block_slot Slot relative to block.
         * @return uint64_t Value present at given slot.
         */
        uint64_t get_rem_block(uint8_t * block_start, uint64_t block_slot);

        /**
         * @brief Set the remainder at a given absolute slot position.
         * 
         * @param slot Absolute slot.
         * @param rem Remainder to be set.
         */
        void set_rem(uint32_t slot, uint64_t rem);
        
        /**
         * @brief Get the remainder at a given absolute slot position.
         * 
         * @param slot Absolute slot.
         * @return uint64_t Remainder at that slot.
         */
        uint64_t get_rem(uint32_t slot);

        /**
         * @brief (Single block only) Given a block and a relative slot, 
         *        shift all the remainders one slot to the right from said slot.
         * 
         * @param block_start Pointer to the start of the block
         * @param insertion_slot Relative slot position
         */
        void shift_right_from(uint8_t * block_start, uint64_t insertion_slot);

        /**
         * @brief (Single block only) Returns the absolute slot at which a given remainder should be inserted in a run. Sorted in ascending order.
         * 
         * @param block_start Pointer to the start of the block.
         * @param run_start_slot Absolute slot corresponding to the start of the run at which the remainder will be inserted.
         * @param rem Remainder for comparison.
         * @return uint64_t Slot at which to insert the given remainder.
         */
        uint64_t find_insert_slot(uint8_t * block_start, uint64_t run_start_slot, uint64_t rem);

        /**
         * @brief (Single block only) Finds the start of a run corresponding to a given absolute slot. If there is no run returns slot.
         * 
         * @param block_start Pointer to the start of the block where the given absolute slot is in.
         * @param slot Absolute slot.
         * @return uint64_t Start of a run corresponding to a given slot or the slot itself.
         */
        uint64_t find_run_start(uint8_t * block_start, uint64_t slot);

        /**
         * @brief Sets all the values in a CQF to zero.
         * 
         */
        void reset();
};
#endif