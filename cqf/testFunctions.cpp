#include <testFunctions.hpp>
#include <CountingQF.hpp>
#include <assert.h>
#include <bitset>

#define GREEN "\033[32m"
#define RESET "\033[0m"

bool testAsmRank(CountingQF cqf)
{
    assert(cqf.asmRank(6840554685586519, 63) == 25);
    std::cout << GREEN << "asmRankO1 OK" << RESET << '\n';
    
    assert(cqf.asmRank(8162520098696294, 0) == 0);
    std::cout << GREEN << "asmRankO2 OK" << RESET <<  '\n';
    
    assert(cqf.asmRank(851508182385204, 2) == 1);
    std::cout << GREEN << "asmRankO3 OK" << RESET << '\n';

    assert(cqf.asmRank(0, 32) == 0);

    std::cout << GREEN << "asmRankO4 OK" << RESET << '\n';
    std::cout << std::endl;
    return true;
}

bool testAsmSelect(CountingQF cqf)
{
    // 64 if fewer 1s than wanted.
    assert(cqf.asmSelect(6840554685586519, 63) == 64);
    std::cout << GREEN << "asmSelectO1 OK" << RESET << '\n';
    
    assert(cqf.asmSelect(8162520098696294, 0) == 1);
    std::cout << GREEN << "asmSelectO2 OK" << RESET <<  '\n';
    
    assert(cqf.asmSelect(851508182385204, 2) == 5);
    std::cout << GREEN << "asmSelectO3 OK" << RESET << '\n';
    std::cout << std::endl;
    return true;
}

bool testGetRemBlock(CountingQF cqf)
{
    uint8_t * blockAddr = cqf.qf;
    int uintOffsetZero = cqf.remainderPos[0][0];
    int uintOffsetTwo = cqf.remainderPos[2][0];
    blockAddr += 17 +  uintOffsetZero;

    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    blockAddr -= 20;
    cqf.getRemFromBlock(0, blockAddr);
    std::cout << GREEN << "getRemBlock01 OK" << RESET << '\n';

    blockAddr += 17 + uintOffsetTwo;
// 11111111 11111111 11111111 11111111

    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    blockAddr++;
    *blockAddr = (uint8_t) 0xff;
    
    blockAddr -= 17 + 13 + 3;

    cqf.getRemFromBlock(2, blockAddr);

    //std::cout << std::bitset<64>(cqf.getRemFromBlock(2, blockAddr)) << RESET << '\n';
    std::cout << std::endl;
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