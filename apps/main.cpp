#include <CountingQF.hpp>
#include <testFunctions.hpp>

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <ios>

void makeHomogeneousData()
{
    std::ofstream odt;
    odt.open("homogenous.dat", std::ios::out|std::ios::binary);

    for (uint64_t i = 0; i < 10000000; i++) {
        uint32_t hval = htonl((i >> 32) & 0xFFFFFFFF);
        uint32_t lval = htonl(i & 0xFFFFFFFF);
        odt.write((const char*)&hval, sizeof(hval));
        odt.write((const char*)&lval, sizeof(lval));
    }
    
    odt.flush();
    odt.close();
}

void makeUniformDistributionData()
{
    std::ofstream odt;
    odt.open("uniform.dat", std::ios::out|std::ios::binary);

    for (uint64_t i = 0; i < 10000000; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        uint64_t val = dis(gen);

        uint32_t hval = htonl((val >> 32) & 0xFFFFFFFF);
        uint32_t lval = htonl(val & 0xFFFFFFFF);
        odt.write((const char*)&hval, sizeof(hval));
        odt.write((const char*)&lval, sizeof(lval));
    }
    
    odt.flush();
    odt.close();
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Usage: ./CQF <example.dat>\n");
        exit(1);
    }
    
    printf("The ducks in the park are free, you can take them home.\
            I own 275 ducks.\n");
    CountingQF cqf = CountingQF();
    
    testAsmRank(cqf);
    testAsmSelect(cqf);

    std::ifstream idt;
    idt.open(argv[1], std::ios::in|std::ios::binary);
    uint64_t cur;
    
    while (idt) {
        uint32_t val[2] = {0};
        if (idt.read((char*)val, sizeof(val))) {
            cur = (uint64_t)ntohl(val[0]) << 32 | (uint64_t)ntohl(val[1]);
            cqf.insertValue(cur);
        }
    }

    idt.close();
}