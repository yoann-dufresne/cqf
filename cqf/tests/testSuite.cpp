#include <testSuite.hpp>
#define MEM_UNIT 8
#define MAX_UINT 64

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

TEST_CASE("Setting a byte with a mask using the set8() function")
{
    uint8_t * qf = new uint8_t[256];
    memset(qf, 0, 256);

    set8(qf + 12, 0b11110110, 0b10101001);
    REQUIRE(*(qf + 12) == 0b10100000u);
    
    set8(qf + 12, 0b11111111, 0b11100000);
    REQUIRE(*(qf + 12) == 0b11100000);

    set8(qf + 42, 0b11110110, 0b00001111);
    REQUIRE(*(qf + 42) == 0b00000110);

    set8(qf + 44, 0b00000000, 0b10101001);
    REQUIRE(*(qf + 44) == 0b0000000);

    set8(qf + 250, 0b01010101, 0b11111100);
    REQUIRE(*(qf + 250) == 0b01010100);

    set8(qf + 250, 0b00000011, 0b11111111);
    REQUIRE(*(qf + 250) == 0b00000011);

    delete[] qf;
}


// When initializing with large values such as 29.
// We get a Set address large perms - Valgrind Warning.
// Meaning a large block of memory has been operated on.
// Nothing to worry about.

SCENARIO("CQF is created with the correct dimensions")
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

    WHEN("An empty CQF is initialized at 2^11")
    {
        CountingQF test = CountingQF(11);
        THEN("Has the correct number of slots and the correct total size in bytes") {
            REQUIRE(test.number_of_slots == 2048);
            REQUIRE(test.filter_size / 8 == 14112);
        }

        THEN("Has the correct number of blocks each with the right size in bytes") {
            REQUIRE(test.block_byte_size == 441);
            REQUIRE(test.number_of_blocks == 32);
        }
    }

    WHEN("An empty CQF object is initialized at a random power_of_two value between 6 and 29")
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

using namespace std;
TEST_CASE("Setting and getting the remainders from a multiblock CQF")
{
    // uint64_t rem1 = 0xffffffffffffffffU;
    uint64_t rem2 = 0x5555555555555555U;
    // uint64_t rem3 = 0b1010101011101010101010101010101010101010101010101010101010101010U;
    // uint64_t rem4 = 0xacdbe82901912afdU;

    for (int i = 6; i < 20; i++)
    {
        CountingQF cqf = CountingQF(i);
        uint64_t remainder_mask = (1ULL << cqf.remainder_len) - 1;

    //     cqf.set_rem(0, rem1);
    //     cqf.set_rem(4, rem2);
    //     cqf.set_rem(5, rem3);
    //     cqf.set_rem(6, rem4);
    //     cqf.set_rem(12, rem1);
        cqf.set_rem(63, rem2);

    //     if (i > 9) {
    //         cqf.set_rem(321, rem3);
    //         cqf.set_rem(322, rem4);
    //         cqf.set_rem(667, rem2);
    //     }

    //     if (i > 13) {
    //         cqf.set_rem(4390, rem1);
    //         cqf.set_rem(5352, rem3);
    //         cqf.set_rem(5353, rem4);
    //     }

    //     REQUIRE(cqf.get_rem(0) == (rem1 & remainder_mask));
    //     REQUIRE(cqf.get_rem(4) == (rem2 & remainder_mask));
    //     REQUIRE(cqf.get_rem(5) == (rem3 & remainder_mask));
    //     REQUIRE(cqf.get_rem(6) == (rem4 & remainder_mask));
    //     REQUIRE(cqf.get_rem(12)  == (rem1 & remainder_mask));
        REQUIRE(cqf.get_rem(63)  == (rem2 & remainder_mask));

    //     if (i > 9) {
    //         REQUIRE(cqf.get_rem(321) == (rem3 & remainder_mask));
    //         REQUIRE(cqf.get_rem(322) == (rem4 & remainder_mask));
    //         REQUIRE(cqf.get_rem(667)  == (rem2 & remainder_mask));
    //     }

    //     if (i > 13) {
    //         REQUIRE(cqf.get_rem(4390) == (rem1 & remainder_mask));
    //         REQUIRE(cqf.get_rem(5352) == (rem3 & remainder_mask));
    //         REQUIRE(cqf.get_rem(5353) == (rem4 & remainder_mask));
    //     }
    }   
}

/*
uint64_t rand64()
{
    uint64_t res = random(0, 2147483648U);
    res <<= 32;
    res += random(0, 2147483648U);
    return (res);
}

TEST_CASE("Setting and getting the remainders from a multiblock CQF")
{
    uint32_t size = GENERATE(take(25, filter([](int i) { return i; }, random(6,29))));
    
    CountingQF cqf = CountingQF(size);
    
    uint64_t remainder_mask = (1ULL << cqf.remainder_len) - 1;

    uint64_t insertions = 100;    
    for (int i = 0; i < insertions; i++)
    {
        uint64_t rem = rand64();
    }
}
*/