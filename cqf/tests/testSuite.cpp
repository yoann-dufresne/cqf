#include <testSuite.hpp>
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

// TO-DO: Resize scenario in the future
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
/*
0000000000
11111111111111111111111111111111111111111111
000011111111111111111111111111111111111111111111111111
*/
using namespace std;
TEST_CASE("Setting and getting the remainders from a multiblock CQF")
{
    CountingQF cqf = CountingQF(10);

    // Ignore bit_offset left bits
    // Ignore next slot bit_offset right bits
    cqf.set_rem_rev(cqf.qf, 1, 0xffffffffffffffffU);
    cqf.set_rem_rev(cqf.qf, 5, 0xffffffffffffffffU);
    cqf.set_rem_rev(cqf.qf, 12, 0xffffffffffffffffU);    
    cout << bitset<64>(cqf.get_rem_rev(cqf.qf, 12)) << endl;
/*
    cout << bitset<8>(*(cqf.qf + 17 + 13)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 14)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 15)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 16)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 17)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 18)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 19)) << endl;
    cout << bitset<8>(*(cqf.qf + 17 + 20)) << endl;
*/
    REQUIRE(cqf.get_rem_rev(cqf.qf,1) == 0x0fffffffffffffc0);
    REQUIRE(cqf.get_rem_rev(cqf.qf,5) == 0x0fffffffffffffc0);
    REQUIRE(cqf.get_rem_rev(cqf.qf,12) == 0x0fffffffffffffc0);
}