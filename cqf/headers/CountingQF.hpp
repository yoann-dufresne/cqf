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
         * @brief Prints all of the CQF, blocks and remainders.
         * 
         */
        void print_CQF();

        /**
         * @brief Prints block pointed to by block_start.
         * 
         */
        void print_block(uint8_t * block_start);

        /**
         * @brief Prints a remainder that is at the address of rem_addr
         * bitsToSkip bits after the first one.
         * 
         * @param rem_addr Pointer to the address at the start of the remainder.
         * @param bits_to_skip Number of bits to skip at rem_addr to get to the
         * start of the remainder.
         */
        void print_remainder(uint8_t * rem_addr, uint8_t bits_to_skip);

         /**
         * @brief Set the remainder at the slot relative to block_start
         * to the value of rem.
         * 
         * @param block_start Block to set a value in.
         * @param slot Slot relative to the block.
         * @param rem remainder to set the value as.
         */
        void set_rem(uint8_t * block_start, uint32_t slot, uint64_t rem);
        void set_rem_rev(uint8_t * block_start, uint32_t slot, uint64_t rem);
        uint64_t get_rem(uint8_t * block_start, uint32_t slot);
        uint64_t get_rem_rev(uint8_t * block_start, uint32_t slot);
};
#endif