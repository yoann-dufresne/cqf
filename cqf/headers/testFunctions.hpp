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
bool testAsmRank(CountingQF cqf);

/**
 * @brief  Returns index of the nth 1 in val
 *
 * @param cqf CQF for testing.
 * @return true OK
 * @return false KO
 */
bool testAsmSelect(CountingQF cqf);

/**
 * @brief Times asmRank function calls
 * 
 * @param cqf CQF for testing
 */
void timeAsmRank(CountingQF cqf);

/**
 * @brief Times asmSelect function calls
 * 
 * @param cqf CQF for testing
 */
void timeAsmSelect(CountingQF cqf);

bool testGetRemBlock(CountingQF cqf);
#endif