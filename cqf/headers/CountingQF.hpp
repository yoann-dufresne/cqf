#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <iterator>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;
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

        struct Iterator {
            // Forward iterator type
            using iterator_category = forward_iterator_tag;

            // Using pointer arithmetics on our QF array
            using difference_type = ptrdiff_t;

            // We're iterating over the inserted values
            using value_type = uint64_t;
            
            // Pointing to uint8_ts in our qf.
            using pointer = uint8_t*;
            using reference = uint8_t&;

        /*
            Iterator(pointer ptr) : m_ptr(ptr) {}

            reference operator*() const { return *m_ptr; }
            pointer operator->() const { return m_ptr; }
            Iterator& operator++() { m_ptr++; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }
        */
        public:
            pointer block_ptr;
            pointer mem_unit_ptr;
            uint64_t curr_slot_rel;
            uint64_t curr_slot_abs;
        };

        // Iterator begin() { return Iterator(&qf[0]); }
        // Iterator end() { return Iterator(&qf[block_byte_size * number_of_blocks]); }
};
#endif