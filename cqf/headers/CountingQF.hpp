#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cmath>

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
         * @brief Insert an already hashed value into the CQF.
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
};
#endif