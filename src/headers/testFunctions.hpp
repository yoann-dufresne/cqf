#ifndef TESTSUITE_HPP
#define TESTSUITE_HPP

#include <cstdint>
#include <cmath>

/**
 * @brief  Test if CQF asmRank funcition returns the number of 1s in
 * in uint64_t val up to position i including
 * @param val Testing Value
 * @param i Up to this index
 * @return true OK
 * @return false KO
 */
bool testAsmRank(uint64_t val, int i);

/**
 * @brief  Returns index of the nth 1 in val
 *
 * @param val Testing Value
 * @param i Up to this index
 * @return true OK
 * @return false KO
 */
bool testAsmSelect(uint64_t val, int n);

#endif