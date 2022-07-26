#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#define MEM_UNIT 8
#define MAX_UINT 64

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
            
            // Pointing to values in our qf.
            using pointer = uint8_t*;
            using reference = uint8_t&;

        
            Iterator(CountingQF * c) {
                cqf = c;
                block_ptr = cqf -> qf;
                mem_unit_ptr = cqf -> qf + 17;
                curr_slot_abs = 0;
                curr_slot_rel = 0;
                curr_val = 0;
            }

            reference operator*() const {
                return (*mem_unit_ptr);    
            }

            pointer operator->() const {
                return (mem_unit_ptr);
            }
        
            Iterator& operator++() {
                uint64_t bits_left = cqf -> remainder_len;
                uint64_t first_bit_offset = (curr_slot_rel * cqf -> remainder_len) % MEM_UNIT; 

                curr_slot_rel += 1;
                curr_slot_abs += 1;

                bits_left -= first_bit_offset;
                mem_unit_ptr += 1;

                while (bits_left > 8) {          
                    mem_unit_ptr += 1;
                    bits_left -= MEM_UNIT;
                }

                if (curr_slot_rel > 63) {
                    curr_slot_rel %= MAX_UINT;
                    mem_unit_ptr += 17;
                }
                
                return *this;
            }
        /*
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }
        */
        public:
            CountingQF * cqf;

            uint8_t * block_ptr;
            uint8_t * mem_unit_ptr;
            
            mutable uint64_t curr_slot_rel;
            mutable uint64_t curr_slot_abs;
            mutable uint64_t curr_val;
        };

        // Iterator begin() { return Iterator(&qf[0]); }
        // Iterator end() { return Iterator(&qf[block_byte_size * number_of_blocks]); }
};
#endif