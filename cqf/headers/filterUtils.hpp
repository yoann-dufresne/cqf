#ifndef FILTERUTILS_HPP
#define FILTERUTILS_HPP

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cmath>

    /**
    * @brief Returns number of set bits in val up to index i inclusive.
    * 
    * @param val Value to count set bits from.
    * @param i Inclusive upper bound for counting. 
    * @return int Number of set bits in val.
    */
    int asm_select(uint64_t val, int i);

    /**
    * @brief Returns index of the n-th set bit in val, couting from 0.
    * 
    * @param val Value to search set bit in.
    * @param n The rank of a given set bit to look for.
    * @return int Returns index of the n-th set bit if found,
    * returns 64 if there were not enough set bits to go up to rank n.
    */
    int asm_rank(uint64_t val, int n);

    /**
     * @brief Set masked value at position pointed by pointer pos.
     * 
     * @param pos Pointer to the position to set the value in.
     * @param value The value to set it to.
     * @param mask Bit AND mask.
     */
    void set8(uint8_t * pos, uint8_t value, uint8_t mask);

    /**
     * @brief Shifts values from slotStart to slotEnd at block pointed by
     * blockAddr one slot to the right.
     * 
     * @param blockAddr Pointer to block corresponding to the slot at slotStart.
     * @param slotStart Position of the start slot to the shift relative
     * to the block at blockAddr.
     * @param slotEnd Position of the end slot to shift relative to the 
     * block at blockAddr.
     */
    void shift(uint8_t * blockAddr, uint32_t slotStart, uint32_t slotEnd);
    
    /**
     * @brief Shifts masked valueToShift skipping (advanceBy - 1) addresses.
     * 
     * @param valueToShift Pointer by reference to to-shift value.
     * @param advanceBy Number of addresses to skip when shifting
     * (1 normally, 17 when shifting between blocks).
     * @param mask Bit AND mask.
     */
    void shift_right(uint8_t * valueToShift, uint8_t advanceBy, uint8_t mask);
    
    /**
     * @brief Print masked value.
     * 
     * @param value Value to print.
     * @param mask Mask to use.
     */
    void print8(uint8_t value, uint8_t mask);
    
    /**
    * @brief Get the value of the n-th bit from a given uint.
    * 
    * @param vec Value to search n-th bit in.
    * @param n Position of desired bit.
    * @return uint8_t 1 or 0.
    */
    inline uint8_t get_nth_bit_from(uint64_t vec, int n);
        
    /**
     * @brief Set the n-th bit of a given uint by passing it
     * by reference.
     * 
     * @param vec Passed-by-reference value to modify.
     * @param n Position of bit you wish to modify.
     */
    inline void set_nth_bit_from(uint64_t &vec, int n);

    /**
     * @brief Set the n-th bit of a given uint to x by passing it
     * by reference.
     * 
     * @param vec Passed-by-reference value to modify.
     * @param n Position of bit you wish to modify.
     * @param x Value you wish the n-th bit to take, 1 or 0 conventionally.
     */
    inline void  set_nth_bit_to_x(uint64_t &vec, int n, int x);

    /**
     * @brief Unset the n-th bit of a given uint by passing it
     * by reference
     * 
     * @param vec Passed-by-reference value to modify.
     * @param n Position of bit you wish to modify.
     */

    inline void clear_nth_bit_from(uint64_t &vec, int n);

    uint8_t reverse_bits(uint8_t c);

    inline uint8_t get_nth_bit_from(uint64_t vec, int n) {
        return (vec >> n) & 0b1;
    }

    inline void set_nth_bit_from(uint64_t &vec, int n) {
        vec |= 1ULL << n;
    }

    inline void set_nth_bit_to_x(uint64_t &vec, int n, int x) {
        vec ^= (-x ^ vec) & (1ULL << n);
    }

    inline void clear_nth_bit_from(uint64_t &vec, int n) {
        vec &= ~(1ULL << n);
    }

    
#endif