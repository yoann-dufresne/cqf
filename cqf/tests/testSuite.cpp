#include <catch2/catch_test_macros.hpp>
#include "testSuite.hpp"

CountingQF cqf = CountingQF(6);

TEST_CASE("Computing assembly RANK", "[asm_rank]")
{
    REQUIRE(asm_rank(6840554685586519, 63) == 25);
    REQUIRE(asm_rank(8162520098696294, 0) == 0);
    REQUIRE(asm_rank(851508182385204, 2) == 1);
    REQUIRE(asm_rank(0, 32) == 0);
}