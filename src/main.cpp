#include <CountingQF.hpp>
#include <testFunctions.hpp>

#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <bitset>
#include <ios>

int main(int argc, char ** argv)
{
    printf("The ducks in the park are free, you can take them home.\
            I own 275 ducks.\n");
    CountingQF cqf = CountingQF();
    testAsmRank(cqf);
    testAsmSelect(cqf);

    std::ofstream odt;
    odt.open("trash.dat", std::ios::out|std::ios::binary);

    for (uint64_t i = 0; i < 100000000; i++) {
        uint32_t hval = htonl((i >> 32) & 0xFFFFFFFF);
        uint32_t lval = htonl(i & 0xFFFFFFFF);
        odt.write((const char*)&hval, sizeof(hval));
        odt.write((const char*)&lval, sizeof(lval));
    }
    
    odt.flush();
    odt.close();


    std::ifstream idt;
    idt.open("trash.dat", std::ios::in|std::ios::binary);
    uint64_t cur = 0;
    
    while (idt) {
        uint32_t val[2] = {0};
        if (idt.read((char*)val, sizeof(val))) {
            cur = (uint64_t)ntohl(val[0]) << 32 | (uint64_t)ntohl(val[1]);
            cqf.insertValue(cur);
        }
    }

    idt.close();
}