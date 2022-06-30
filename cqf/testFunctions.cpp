#include <testFunctions.hpp>
#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <assert.h>
#include <bitset>

#define GREEN "\033[32m"
#define RESET "\033[0m"
#define MEM_UNIT 8
#define MAX_UINT 64

bool test_asm_rank(CountingQF cqf)
{
    assert(asm_rank(6840554685586519, 63) == 25);
    std::cout << GREEN << "asm_rankO1 OK" << RESET << '\n';
    
    assert(asm_rank(8162520098696294, 0) == 0);
    std::cout << GREEN << "asm_rankO2 OK" << RESET <<  '\n';
    
    assert(asm_rank(851508182385204, 2) == 1);
    std::cout << GREEN << "asm_rankO3 OK" << RESET << '\n';

    assert(asm_rank(0, 32) == 0);
    std::cout << GREEN << "asm_rankO4 OK" << RESET << '\n';
    std::cout << std::endl;
    
    return true;
}

bool test_asm_select(CountingQF cqf)
{
    // 64 if fewer 1s than wanted.
    assert(asm_select(6840554685586519, 63) == 64);
    std::cout << GREEN << "asm_selectO1 OK" << RESET << '\n';
    
    assert(asm_select(8162520098696294, 0) == 1);
    std::cout << GREEN << "asm_selectO2 OK" << RESET <<  '\n';
    
    assert(asm_select(851508182385204, 2) == 5);
    std::cout << GREEN << "asm_selectO3 OK" << RESET << '\n';
    std::cout << std::endl;
    
    return true;
}

using namespace std;
bool test_set_rem(CountingQF cqf)
{
    uint64_t remToSet = 0b0101010100000001000000010000000100000001000000010000000100000001;
    uint8_t slot = 6;

    cqf.set_rem_rev(cqf.qf, slot, remToSet);

    uint64_t * remainder = (uint64_t *) malloc(64);
    uint8_t * slot_addr = cqf.qf + 17 + ((cqf.remainder_len * slot) / 8);

    memcpy(remainder, slot_addr, 8);
    
    if (*remainder == cqf.get_rem_rev(cqf.qf, slot))
        cout << GREEN << "set_rem_rev00 OK" << RESET << endl;
   
    free(remainder);
    return true;
}
/*
0100000001000000010000000100000001000000010000000101000000
0100000001000000010000000100000001000000010000000101000000
10000000
10000000
10000000
10000000
10000000
10000000
10000000
10

00000001
00000001
00000001
00000001
00000001
00000001
00000001
01
*/
bool testGetRemBlock(CountingQF cqf)
{
    return true;
}

//~1300-2300µs for 100000 function calls with random values.
// asm_rank(0 - 10^15, 0 - 64)
void time_asm_rank(CountingQF cqf)
{
    auto start = std::chrono::high_resolution_clock::now();
    // Your calls here
    auto end = std::chrono::high_resolution_clock::now();
    
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end-start);

    std::cout << "timeasm_rank: " << microseconds.count() << "µs\n";
}

//~2000-4000µs for 100000 function calls with random values.
// asm_select(0 - 10^15, 0 - 64)
void time_asm_select(CountingQF cqf)
{
    auto start = std::chrono::high_resolution_clock::now();
    // Your calls here
         
    auto end = std::chrono::high_resolution_clock::now();
    
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end-start);

    std::cout << "timeasm_select: " << microseconds.count() << "µs\n";

}