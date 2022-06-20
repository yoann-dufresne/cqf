#include <testFunctions.hpp>
#include <CountingQF.hpp>
#include <assert.h>

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

    return true;
}

bool testGetRemBlock(CountingQF cqf)
{
    //int pos = cqf.remainderPos[2][0];

    uint8_t * blockAddr = cqf.qf;

    //assert(cqf.getRemFromBlock(2, blockAddr) == 0);
    //std::cout << GREEN << "remBlock01 OK" << RESET << '\n';

    for (int i = 0; i < 64; i++)
        cqf.setRemAtBlock(0x00, i, blockAddr);

    //assert(cqf.getRemFromBlock(2, blockAddr) == 0b101);
    std::cout << cqf.getRemFromBlock(2, blockAddr) << std::endl;
    cqf.printCQFrems();

    //std::cout << GREEN << "remBlock02 OK" << RESET << '\n';

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