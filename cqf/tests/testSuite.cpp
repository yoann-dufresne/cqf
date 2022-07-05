#include "testSuite.hpp"
#define MEM_UNIT 8
#define MAX_UINT 64

CountingQF cqf = CountingQF(6);

TEST_CASE("Computing assembly RANK", "[asm_rank]")
{
    REQUIRE(asm_rank(6840554685586519, 63) == 25);
    REQUIRE(asm_rank(8162520098696294, 0) == 0);
    REQUIRE(asm_rank(851508182385204, 2) == 1);
    REQUIRE(asm_rank(0, 32) == 0);
}

TEST_CASE("Computing assembly SELECT", "[asm_select]")
{
    REQUIRE(asm_select(6840554685586519, 63) == 64);
    REQUIRE(asm_select(8162520098696294, 0) == 1);
    REQUIRE(asm_select(851508182385204, 2) == 5);
    REQUIRE(asm_select(0, 32) == 64);
}

// TO-DO: Resize scenario in the future
SCENARIO("CQF Filter is created with the correct dimensions")
{
    WHEN("An empty CQF is initialized at 2^6")
    {
        CountingQF test = CountingQF(6);
        THEN("Has the correct number of slots and the correct total size in bytes") {
            REQUIRE(test.number_of_slots == 64);
            REQUIRE(test.filter_size / 8 == 481);
        }

        THEN("Has the correct number of blocks each with the right size in bytes") {
            REQUIRE(test.block_byte_size == 481);
            REQUIRE(test.number_of_blocks == 1);
        }
    }
    
    WHEN("An empty CQF is initialized at a random power_of_two value between 6 and 29")
    {
        auto power_of_two = GENERATE(take(1, random(6, 29)));
        CountingQF test = CountingQF(power_of_two);

        uint32_t nblocks = (test.number_of_slots / MAX_UINT);
        nblocks = (nblocks == 0) ? nblocks+1 : nblocks;
        
        THEN("Has the correct number of slots and the correct total size in bytes") {
            REQUIRE(test.number_of_slots == (1ULL << power_of_two));
            
            REQUIRE(test.filter_size == (8 * nblocks) + ((MAX_UINT * 2) * nblocks) + ((MAX_UINT - power_of_two) * test.number_of_slots));
        }

        THEN("Has the correct number of blocks each with the right size in bytes") {
            REQUIRE(test.number_of_blocks == nblocks);
            REQUIRE(test.block_byte_size == test.filter_size / test.number_of_blocks / 8);
        }
    }
}