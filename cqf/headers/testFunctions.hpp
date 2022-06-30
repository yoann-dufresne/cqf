#ifndef TESTSUITE_HPP
#define TESTSUITE_HPP

#include <CountingQF.hpp>
#include <cstdint>
#include <cmath>
#include <chrono>

/**
 * @brief  Test if CQF asmRank funcition returns the number of 1s in
 * uint64_t val up to position i including
 * @param cqf CQF for testing.
 * @return true OK
 * @return false KO
 */
bool test_asm_rank(CountingQF cqf);

/**
 * @brief  Returns index of the nth 1 in val
 *
 * @param cqf CQF for testing.
 * @return true OK
 * @return false KO
 */
bool test_asm_select(CountingQF cqf);

/**
 * @brief Times asmRank function calls
 * 
 * @param cqf CQF for testing
 */
void time_asm_rank(CountingQF cqf);

/**
 * @brief Times asmSelect function calls
 * 
 * @param cqf CQF for testing
 */
void time_asm_select(CountingQF cqf);

bool test_get_rem_block(CountingQF cqf);

bool test_set_rem(CountingQF cqf);
#endif