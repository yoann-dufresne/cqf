#include <testSuite.hpp>
#define MEM_UNIT 8
#define MAX_UINT 64

using namespace std;

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
        nblocks = (nblocks == 0) ? 1 : nblocks;
        
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

TEST_CASE("Setting and getting the remainders from a multiblock CQF")
{
    uint64_t rem1 = 0xffffffffffffffffU;
    uint64_t rem2 = 0x5555555555555555U;
    uint64_t rem3 = 0xaaaaaaaaaaaaaaaaU;
    uint64_t rem4 = 0xacdbe82901912afdU;

    for (int i = 6; i < 20; i++)
    {
        CountingQF cqf = CountingQF(i);
        uint64_t remainder_mask = (1ULL << cqf.remainder_len) - 1;

        cqf.set_rem(0, rem1);
        cqf.set_rem(4, rem2);
        cqf.set_rem(5, rem3);
        cqf.set_rem(6, rem4);
        cqf.set_rem(12, rem1);
        cqf.set_rem(63, rem2);

        if (i > 9) {
            cqf.set_rem(321, rem3);
            cqf.set_rem(322, rem4);
            cqf.set_rem(667, rem2);
        }

        if (i > 13) {
            cqf.set_rem(4390, rem1);
            cqf.set_rem(5352, rem3);
            cqf.set_rem(5353, rem4);
        }

        REQUIRE(cqf.get_rem(0) == (rem1 & remainder_mask));
        REQUIRE(cqf.get_rem(4) == (rem2 & remainder_mask));
        REQUIRE(cqf.get_rem(5) == (rem3 & remainder_mask));
        REQUIRE(cqf.get_rem(6) == (rem4 & remainder_mask));
        REQUIRE(cqf.get_rem(12)  == (rem1 & remainder_mask));
        REQUIRE(cqf.get_rem(63)  == (rem2 & remainder_mask));

        if (i > 9) {
            REQUIRE(cqf.get_rem(321) == (rem3 & remainder_mask));
            REQUIRE(cqf.get_rem(322) == (rem4 & remainder_mask));
            REQUIRE(cqf.get_rem(667)  == (rem2 & remainder_mask));
        }

        if (i > 13) {
            REQUIRE(cqf.get_rem(4390) == (rem1 & remainder_mask));
            REQUIRE(cqf.get_rem(5352) == (rem3 & remainder_mask));
            REQUIRE(cqf.get_rem(5353) == (rem4 & remainder_mask));
        }
    }   
}

SCENARIO("Inserting a value into the CQF") {
    CountingQF cqf = CountingQF(12);
    CountingQF cqf2 = CountingQF(15);

    uint64_t rem1 = 0xffffffffffffffffU;
    uint64_t rem2 = 0x5555555555555555U;
    uint64_t rem3 = 0xfdbbcb756410000cU;
    uint64_t rem4 = 0xacdbe82901912afdU;

    uint8_t * block1 = cqf.qf  + (cqf.block_byte_size * ((rem1 >> cqf.remainder_len) / MAX_UINT));
    uint8_t * block2 = cqf.qf  + (cqf.block_byte_size * ((rem2 >> cqf.remainder_len) / MAX_UINT));
    uint8_t * block3 = cqf.qf  + (cqf.block_byte_size * ((rem3 >> cqf.remainder_len) / MAX_UINT));
    uint8_t * block4 = cqf.qf  + (cqf.block_byte_size * ((rem4 >> cqf.remainder_len) / MAX_UINT));
    uint8_t * block5 = cqf2.qf + (cqf2.block_byte_size * ((rem1 >> cqf2.remainder_len) / MAX_UINT));
    uint8_t * block6 = cqf2.qf + (cqf2.block_byte_size * ((rem2 >> cqf2.remainder_len) / MAX_UINT));
    uint8_t * block7 = cqf2.qf + (cqf2.block_byte_size * ((rem3 >> cqf2.remainder_len) / MAX_UINT));
    uint8_t * block8 = cqf2.qf + (cqf2.block_byte_size * ((rem4 >> cqf2.remainder_len) / MAX_UINT));

    uint8_t * occ_addr1 = block1 + 1;
    uint8_t * run_addr1 = block1 + 9;
    uint8_t * occ_addr2 = block2 + 1;
    uint8_t * run_addr2 = block2 + 9;
    uint8_t * occ_addr3 = block3 + 1;
    uint8_t * run_addr3 = block3 + 9;
    uint8_t * occ_addr4 = block4 + 1;
    uint8_t * run_addr4 = block4 + 9;

    uint8_t * occ_addr5 = block5 + 1;
    uint8_t * run_addr5 = block5 + 9;
    uint8_t * occ_addr6 = block6 + 1;
    uint8_t * run_addr6 = block6 + 9;
    uint8_t * occ_addr7 = block7 + 1;
    uint8_t * run_addr7 = block7 + 9;
    uint8_t * occ_addr8 = block8 + 1;
    uint8_t * run_addr8 = block8 + 9;

    uint64_t rem_mask  = (1ULL << cqf.remainder_len) - 1;
    uint64_t rem_mask2 = (1ULL << cqf2.remainder_len) - 1;

    uint64_t rem_slot1 = rem1 >> cqf.remainder_len;
    uint64_t rem_slot2 = rem2 >> cqf.remainder_len;
    uint64_t rem_slot3 = rem3 >> cqf.remainder_len;
    uint64_t rem_slot4 = rem4 >> cqf.remainder_len;

    uint64_t rem_slot5 = rem1 >> cqf2.remainder_len;
    uint64_t rem_slot6 = rem2 >> cqf2.remainder_len;
    uint64_t rem_slot7 = rem3 >> cqf2.remainder_len;
    uint64_t rem_slot8 = rem4 >> cqf2.remainder_len;

    WHEN("We insert a value without a collision") {
        cqf.insert_value(rem1);
        cqf.insert_value(rem2);
        cqf.insert_value(rem3);
        cqf.insert_value(rem4);

        cqf2.insert_value(rem1);
        cqf2.insert_value(rem2);
        cqf2.insert_value(rem3);
        cqf2.insert_value(rem4);

        THEN("Occupieds and Runends are properly set at the correct slots") {
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr1), rem_slot1) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr2), rem_slot2) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr3), rem_slot3) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr4), rem_slot4) == 1);

            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr1), rem_slot1) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr2), rem_slot2) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr3), rem_slot3) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr4), rem_slot4) == 1);

            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr5), rem_slot5) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr6), rem_slot6) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr7), rem_slot7) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)occ_addr8), rem_slot8) == 1);
            
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr5), rem_slot5) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr6), rem_slot6) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr7), rem_slot7) == 1);
            REQUIRE(get_nth_bit_from(*((uint64_t *)run_addr8), rem_slot8) == 1);
        }

        THEN("Remainder is properly set at the correct slot") {
            REQUIRE(cqf.get_rem(rem_slot1) == (rem1 & rem_mask));
            REQUIRE(cqf.get_rem(rem_slot2) == (rem2 & rem_mask));
            REQUIRE(cqf.get_rem(rem_slot3) == (rem3 & rem_mask));
            REQUIRE(cqf.get_rem(rem_slot4) == (rem4 & rem_mask));

            REQUIRE(cqf2.get_rem(rem_slot5) == (rem1 & rem_mask2));
            REQUIRE(cqf2.get_rem(rem_slot6) == (rem2 & rem_mask2));
            REQUIRE(cqf2.get_rem(rem_slot7) == (rem3 & rem_mask2));
            REQUIRE(cqf2.get_rem(rem_slot8) == (rem4 & rem_mask2));
        }
    }
}
