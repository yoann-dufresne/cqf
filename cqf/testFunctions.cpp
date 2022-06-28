#include <testFunctions.hpp>
#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <assert.h>
#include <bitset>

#define GREEN "\033[32m"
#define RESET "\033[0m"
#define MEM_UNIT 8
#define MAX_UINT 64

bool testAsmRank(CountingQF cqf)
{
    assert(asmRank(6840554685586519, 63) == 25);
    std::cout << GREEN << "asmRankO1 OK" << RESET << '\n';
    
    assert(asmRank(8162520098696294, 0) == 0);
    std::cout << GREEN << "asmRankO2 OK" << RESET <<  '\n';
    
    assert(asmRank(851508182385204, 2) == 1);
    std::cout << GREEN << "asmRankO3 OK" << RESET << '\n';

    assert(asmRank(0, 32) == 0);

    std::cout << GREEN << "asmRankO4 OK" << RESET << '\n';
    std::cout << std::endl;
    return true;
}

bool testAsmSelect(CountingQF cqf)
{
    // 64 if fewer 1s than wanted.
    assert(asmSelect(6840554685586519, 63) == 64);
    std::cout << GREEN << "asmSelectO1 OK" << RESET << '\n';
    
    assert(asmSelect(8162520098696294, 0) == 1);
    std::cout << GREEN << "asmSelectO2 OK" << RESET <<  '\n';
    
    assert(asmSelect(851508182385204, 2) == 5);
    std::cout << GREEN << "asmSelectO3 OK" << RESET << '\n';
    std::cout << std::endl;
    return true;
}

using namespace std;
bool testSetRem(CountingQF cqf)
{
    cqf.setRem(cqf.qf, 0, 0b0000000100000001000000010000000100000001000000010000000101);
    cqf.setRem(cqf.qf, 1, 0b0000000100010001000000010000000100000001000000010000000111);
    cqf.setRem(cqf.qf, 2, 0b0000000100000001001100010000000100000001000000010000000101);
    cqf.setRem(cqf.qf, 6, 0b0100000010000000100000001001000100011001011100010000000101);
    
    printbits(cqf.qf + 17 + (cqf.remainderLen * 0) / 8, 0, cqf.remainderLen);
    printbits(cqf.qf + 17 + (cqf.remainderLen * 1) / 8, 0, cqf.remainderLen);
    printbits(cqf.qf + 17 + (cqf.remainderLen * 2) / 8, 0, cqf.remainderLen);
    printbits(cqf.qf + 17 + (cqf.remainderLen * 6) / 8, 0, cqf.remainderLen);
    
    return true;
}
/*
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
// asmRank(0 - 10^15, 0 - 64)
void timeAsmRank(CountingQF cqf)
{
    auto start = std::chrono::high_resolution_clock::now();
    // Your calls here
    auto end = std::chrono::high_resolution_clock::now();
    
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end-start);

    std::cout << "timeAsmRank: " << microseconds.count() << "µs\n";
}

//~2000-4000µs for 100000 function calls with random values.
// asmSelect(0 - 10^15, 0 - 64)
void timeAsmSelect(CountingQF cqf)
{
    auto start = std::chrono::high_resolution_clock::now();
    // Your calls here
         
    auto end = std::chrono::high_resolution_clock::now();
    
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end-start);

    std::cout << "timeAsmSelect: " << microseconds.count() << "µs\n";

}