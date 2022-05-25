#include <CountingQF.hpp>
#include <testFunctions.hpp>
#include <cstdio>
#include <iostream>
#include <bitset>

int main(int argc, char ** argv)
{
    printf("The ducks in the park are free, you can take them home.\
            I own 275 ducks.\n");
    CountingQF cqf = CountingQF(64);
    testAsmRank(cqf);
    testAsmSelect(cqf);
}