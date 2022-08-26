/**
 * @file filterUtils.hpp
 * @brief Simple operations to manipulate filter data
 * 
 */
#ifndef FILTERUTILS_HPP
#define FILTERUTILS_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
/**
* @brief Returns index of the n-th set bit in val, couting from 0.
* 
* @param val Value to search set bit in.
* @param i The rank of a given set bit to look for.
* @return int Returns index of the n-th set bit if found,
* returns 64 if there were not enough set bits to go up to rank n.
*/
int asm_select(uint64_t val, int i);

/**
* @brief Returns number of set bits in val up to index n inclusive.
* 
* @param val Value to count set bits from.
* @param n Inclusive upper bound for counting. 
* @return int Number of set bits in val.
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
* @brief Get the value of the n-th bit from a given uint.
* 
* @param vec Value to search n-th bit in.
* @param n Position of desired bit.
* @return uint8_t 1 or 0.
*/
uint8_t get_nth_bit_from(uint64_t vec, int n);
    
/**
 * @brief Set the n-th bit of a given uint by passing it
 * by reference.
 * 
 * @param vec Passed-by-reference value to modify.
 * @param n Position of bit you wish to modify.
 */
void set_nth_bit_from(uint64_t &vec, int n);

/**
 * @brief Set the n-th bit of a given uint to x by passing it
 * by reference.
 * 
 * @param vec Passed-by-reference value to modify.
 * @param n Position of bit you wish to modify.
 * @param x Value you wish the n-th bit to take, 1 or 0 conventionally.
 */
void  set_nth_bit_to_x(uint64_t &vec, int n, int x);

/**
 * @brief Unset the n-th bit of a given uint by passing it
 * by reference
 * 
 * @param vec Passed-by-reference value to modify.
 * @param n Position of bit you wish to modify.
 */
void clear_nth_bit_from(uint64_t &vec, int n);

#endif