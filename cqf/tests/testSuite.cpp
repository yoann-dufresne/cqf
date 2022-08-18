#include <testSuite.hpp>
#define MEM_UNIT 8
#define MAX_UINT 64

using namespace std;

TEST_CASE("Computing assembly RANK", "[asm_rank]")
{
    //                 v Counts 25 ones up to pos 63
    REQUIRE(asm_rank(0b11000010011010111001011000000000110011101110001010111, 63) == 25);

    //                                                                     v 0 ones up to pos 0                 
    REQUIRE(asm_rank(0b11100111111111100010011001110111101000110110001100110, 0) == 0);

    //                                                                   v 1 ones up to pos 2
    REQUIRE(asm_rank(0b00001000001100111000100110010101111000000001000110100, 2) == 1);

    // Counts 0 ones up to pos 32 including
    REQUIRE(asm_rank(0xfffffffe00000000U, 32) == 0);
}

TEST_CASE("Computing assembly SELECT", "[asm_select]")
{   
    //                   v 25th 1 (not enough 1s)
    REQUIRE(asm_select(0b11000010011010111001011000000000110011101110001010111, 63) == 64);

    //                                                                      v This one
    REQUIRE(asm_select(0b11100111111111100010011001110111101000110110001100110, 0) == 1);

    //                                                                  v This one
    REQUIRE(asm_select(0b00001000001100111000100110010101111000000001000110100, 2) == 5);
    
    // Not enough 1's
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

TEST_CASE("Testing bit manipulation functions on an bit vector")
{
    uint64_t rem = 0ULL;

    set_nth_bit_from(rem, 10);
    set_nth_bit_from(rem, 12);
    set_nth_bit_from(rem, 63);
    
    set_nth_bit_to_x(rem, 63, 0);
    set_nth_bit_to_x(rem, 62, 1);
    
    set_nth_bit_from(rem, 11);
    clear_nth_bit_from(rem, 11);

    REQUIRE(get_nth_bit_from(rem, 63) == 0);
    REQUIRE(get_nth_bit_from(rem, 62) == 1);
    REQUIRE(get_nth_bit_from(rem, 11) == 0);
    REQUIRE(get_nth_bit_from(rem, 10) == 1);
    REQUIRE(get_nth_bit_from(rem, 12) == 1);
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
            REQUIRE(test.filter_size == (8 * nblocks) + ((MAX_UINT * 2) * nblocks) + 
                        ((MAX_UINT - power_of_two) * test.number_of_slots));
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

    for (int i = 6; i < 30; i++)
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

SCENARIO("Inserting a value into the CQF")
{
    CountingQF cqf = CountingQF(12);
    CountingQF cqf2 = CountingQF(15);

    uint64_t rem1 = 0xffc1ffffffffffffU;
    uint64_t rem2 = 0x5555555555555555U;
    uint64_t rem3 = 0xfdbbcb756410000cU;
    uint64_t rem4 = 0xacdbe82901912afdU;

    uint64_t rems[4] = {rem1, rem2, rem3, rem4};
    uint8_t * blocks[4];
    uint8_t * blocks2[4];

    for (int i = 0; i < 4; i++) {
        blocks[i] = cqf.qf + (cqf.block_byte_size * ((rems[i] >> cqf.remainder_len) / MAX_UINT));
        blocks2[i] = cqf2.qf + (cqf2.block_byte_size * ((rems[i] >> cqf2.remainder_len) / MAX_UINT));
    }
    
    uint8_t * occupieds[4];
    uint8_t * runends[4];

    uint8_t * occupieds2[4];
    uint8_t * runends2[4];

    for (int i = 0; i < 4; i++) {
        occupieds[i] = blocks[i] + 1;
        runends[i] = blocks[i] + 9;
        occupieds2[i] = blocks2[i] + 1;
        runends2[i] = blocks2[i] + 9;
    }

    uint64_t rem_mask  = (1ULL << cqf.remainder_len) - 1;
    uint64_t rem_mask2 = (1ULL << cqf2.remainder_len) - 1;

    uint64_t slots[4];
    uint64_t slots2[4];

    for (int i = 0; i < 4; i++) {
        slots[i] = rems[i] >> cqf.remainder_len;
        slots2[i] = rems[i] >> cqf2.remainder_len;    
    }

    WHEN("We insert a value without a collision") {
        
        for (int i = 0; i < 4; i++) {
            cqf.insert_value(rems[i]);
            cqf2.insert_value(rems[i]);
        }
        
        THEN("Occupieds and Runends are properly set at the correct slots") {
            for (int i = 0; i < 4; i++) {
                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds[i]), slots[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends[i]), slots[i] % MAX_UINT) == 1);

                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds2[i]), slots2[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends2[i]), slots2[i] % MAX_UINT) == 1);
            }
        }

        THEN("Remainder is properly set at the correct slot") {
            for (int i = 0; i < 4; i++) {
                REQUIRE(cqf.get_rem(slots[i]) == (rems[i] & rem_mask));
                REQUIRE(cqf2.get_rem(slots2[i]) == (rems[i] & rem_mask2));
            }
        }
    }

    cqf.reset();
    cqf2.reset();

    WHEN ("We insert values with a single collision") {
        uint64_t col1 = 0xdeadbeef00000001U;
        uint64_t col2 = 0xdeadbeef00000002U;
        uint64_t col3 = 0xfee00000000b000U;
        uint64_t col4 = 0xfee00000000cafeU;

        uint64_t cols[4] = {col1, col2, col3, col4};
        uint64_t slots[4];
        uint64_t slots2[4];

        for (int i = 0; i < 4; i++) {
            slots[i] = cols[i] >> cqf.remainder_len;
            slots2[i] = cols[i] >> cqf2.remainder_len;
        }
        uint8_t * blocks[4];
        uint8_t * blocks2[4];

        for (int i = 0; i < 4; i++) {
            blocks[i]  = cqf.qf + (cqf.block_byte_size * ((cols[i] >> cqf.remainder_len) / MAX_UINT));
            blocks2[i] = cqf2.qf + (cqf2.block_byte_size * ((cols[i] >> cqf2.remainder_len) / MAX_UINT));
        }

        uint8_t * occupieds[4];
        uint8_t * occupieds2[4];

        uint8_t * runends[4];
        uint8_t * runends2[4];

        for (int i = 0; i < 4; i++) {
            occupieds[i] = blocks[i] + 1;
            occupieds2[i] = blocks2[i] + 1;
            runends[i] = blocks[i] + 9;
            runends2[i] = blocks2[i] + 9;
        }

        for (int i = 0; i < 4; i++) {
            cqf.insert_value(cols[i]);
            cqf2.insert_value(cols[i]);
        }

        THEN("Occupieds and Runends are properly set at the correct slots") {
            for (int i = 0; i < 4; i++) {
                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds[i]), slots[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends[i]), (slots[i] + 1) % MAX_UINT) == 1);

                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds2[i]), slots2[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends2[i]), (slots2[i] + 1) % MAX_UINT) == 1);
            }
        }

        THEN("Remainders are properly sorted") {
            REQUIRE(cqf.get_rem(slots[0]) < cqf.get_rem(slots[0] + 1));
            REQUIRE(cqf.get_rem(slots[2]) < cqf.get_rem(slots[2] + 1));
            REQUIRE(cqf2.get_rem(slots2[0]) < cqf2.get_rem(slots2[0] + 1));
            REQUIRE(cqf2.get_rem(slots2[2]) < cqf2.get_rem(slots2[2] + 1));
        }
    }

    cqf.reset();
    cqf2.reset();

    WHEN ("We insert values with multiple single-block collisions") {
        uint64_t colrun1 = 0xbabe000000000001U;
        uint64_t colrun2 = 0xbabe000000000002U;
        uint64_t colrun3 = 0xbabe000000000003U;
        uint64_t colrun4 = 0xbabe000000000004U;

        uint64_t runsize = 3;

        uint64_t colruns[4] = {colrun1, colrun2, colrun3, colrun4};
        uint64_t slots[4];
        uint64_t slots2[4];

        for (int i = 0; i < 4; i++) {
            slots[i] = colruns[i] >> cqf.remainder_len;
            slots2[i] = colruns[i] >> cqf2.remainder_len;
        }
        uint8_t * blocks[4];
        uint8_t * blocks2[4];

        for (int i = 0; i < 4; i++) {
            blocks[i]  = cqf.qf + (cqf.block_byte_size * ((colruns[i] >> cqf.remainder_len) / MAX_UINT));
            blocks2[i] = cqf2.qf + (cqf2.block_byte_size * ((colruns[i] >> cqf2.remainder_len) / MAX_UINT));
        }

        uint8_t * occupieds[4];
        uint8_t * occupieds2[4];

        uint8_t * runends[4];
        uint8_t * runends2[4];
 
        for (int i = 0; i < 4; i++) {
            occupieds[i] = blocks[i] + 1;
            occupieds2[i] = blocks2[i] + 1;
            runends[i] = blocks[i] + 9;
            runends2[i] = blocks2[i] + 9;
        }

        for (int i = 3; i >= 0; i--) {
            cqf.insert_value(colruns[i]);
            cqf2.insert_value(colruns[i]);
        }

        THEN("Occupieds and Runends are properly set at the correct slots") {
            for (int i = 0; i < 4; i++) {
                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds[i]), slots[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends[i]), (slots[i] + runsize) % MAX_UINT) == 1);

                REQUIRE(get_nth_bit_from(*((uint64_t *)occupieds2[i]), slots2[i] % MAX_UINT) == 1);
                REQUIRE(get_nth_bit_from(*((uint64_t *)runends2[i]), (slots2[i] + runsize) % MAX_UINT) == 1);
            }
        }

        THEN("Remainders are properly sorted") {
            for (int i = 0; i < 3; i++) {
                REQUIRE(cqf.get_rem(slots[0] + i) < cqf.get_rem(slots[0] + (i + 1)));
                REQUIRE(cqf2.get_rem(slots2[0] + i) < cqf2.get_rem(slots2[0] + (i + 1)));
            }
        }

        WHEN ("We insert values on slots that are part of a run but do not belong to said run") {
            uint64_t part1 = 0xbac0000000000005U;
            uint64_t part2 = 0xbac0000000000006U;
            uint64_t part3 = 0xbac0000000000007U;
            uint64_t part4 = 0xbac0000000000008U;

            uint64_t parts[4] = {part1, part2, part3, part4};

            uint64_t col_slots[4];
            
            for (int i = 0; i < 4; i++)
                col_slots[i] = parts[i] >> cqf.remainder_len;

            for (int i = 3; i >= 0; i--) 
                cqf.insert_value(parts[i]);

            THEN ("New runends are set after colliding runend.") {
                for (int i = 0; i < 4; i++)
                    REQUIRE(get_nth_bit_from(*((uint64_t *)runends[i]), (slots[i] + runsize + i)) == 1);

            }

            THEN ("Previous run remainders are still in place and properly sorted") {
                for (int i = 0; i < 3; i++)
                    REQUIRE(cqf.get_rem(slots[0] + i) < cqf.get_rem(slots[0] + (i + 1)));
            }

            THEN ("New run remainders are in place and properly sorted") {
                for (int i = 0; i < 3; i++) {
                    cout << i << endl;
                    REQUIRE(cqf.get_rem(col_slots[0] + (i + 3)) < cqf.get_rem(col_slots[0] + (i + 1 + 3)));
                }
            }

        }
    }
}


// TEST_CASE("Iterating over a CQF with a CQFGetter")
// {
//     CountingQF cqf = CountingQF(9);
//     CountingQF cqf2 = CountingQF(12);

//     const uint64_t rem1 = 0x5555555555555555U;
//     const uint64_t rem2 = 0xacdbe82901912afdU;
//     const uint64_t rem3 = 0xfdbbcb756410000cU;
//     const uint64_t rem4 = 0xffffffffffffffffU;

//     const uint64_t rems[4] = {rem1, rem2, rem3, rem4};

//     cqf.insert_value(rem1);
//     cqf.insert_value(rem2);
//     cqf.insert_value(rem3);
//     cqf.insert_value(rem4);

//     cqf2.insert_value(rem1);
//     cqf2.insert_value(rem2);
//     cqf2.insert_value(rem3);
//     cqf2.insert_value(rem4);

//     CQFGetter getter = CQFGetter(cqf);
//     CQFGetter getter2 = CQFGetter(cqf2);

//     for (int i = 0; i < 3; i++) {
//         REQUIRE(getter.get_current_value() == rems[i]);
//         REQUIRE(getter2.get_current_value() == rems[i]);
        
//         getter.next();
//         getter2.next();
//     }

//     REQUIRE(getter.get_current_value() == rems[3]);
//     REQUIRE(getter2.get_current_value() == rems[3]);
// }

/*
    CountingQF cqf = CountingQF(17);
    CountingQF cqf2 = CountingQF(22);

    auto values = GENERATE(take(10000, filter([](uint64_t i) {return i;}), random(0, 0xffffffffffffffff)));


    for (value : values) {
        cqf.insert_value(value);
        cqf2.insert_value(value);
    }

    CQFGetter getter = CQFGetter(cqf);
    CQFGetter getter2 = CQFGetter(cqf2);
*/

