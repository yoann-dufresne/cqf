#include <CountingQF.hpp>
#include <cstdio>
#include <iostream>
#include <bitset>

int main(int argc, char ** argv)
{
    printf("The ducks in the park are free, you can take them home.\
            I own 275 ducks.\n");


    CountingQF CQF = CountingQF(64);
    printf("q: %d\n", CQF.quotientSize);
    printf("elSize: %d\n", CQF.elementSize);
    printf("r: %ld\n", CQF.remainders.size());
    printf("occupiedsLen: %ld\n", CQF.occupied.size());
}