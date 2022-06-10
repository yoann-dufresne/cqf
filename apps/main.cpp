#include <CountingQF.hpp>

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

    for (uint64_t i = 0; i < 10; i++) {
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

    for (uint64_t i = 0; i < 10; i++) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        uint64_t val = dis(gen);

        printf("Inserted %lu into file", val);
        
        uint32_t hval = htonl((val >> 32) & 0xFFFFFFFF);
        uint32_t lval = htonl(val & 0xFFFFFFFF);
        odt.write((const char*)&hval, sizeof(hval));
        odt.write((const char*)&lval, sizeof(lval));
    }
    
    odt.flush();
    odt.close();
}

// n = 8 = 0b1000
// 7 =     0b0111
// 8 & 7 = 0b0000

// n = 5 = 0b0101
// 4 =     0b0100
// 5 & 4 = 0b0100
bool isPowerOfTwo(int n) {
    return (n != 0 && (n & (n-1)) == 0);
}

int main(int argc, char ** argv)
{
    int cqfSize;

    if (argc != 3 && argc != 2)
    {
        printf("Usage: ./CQF <example.dat> <filterSize>\n");
        printf("    ./CQF <example.dat>\n");
        exit(1);
    }

    cqfSize = atoi(argv[2]);
    CountingQF cqf = CountingQF(cqfSize);

    std::cout << "SLOTS: " << cqf.numberOfSlots << '\n';
    std::cout << "q: " << cqf.quotientLen << '\n';
    std::cout << "r: " << cqf.remainderLen << '\n';
    std::cout << "bits: " << cqf.filterSize << '\n';

    std::ifstream idt;
    idt.open(argv[1], std::ios::in|std::ios::binary);
    //uint64_t cur;

    /*
    while (idt) {
        uint32_t val[2] = {0};
        if (idt.read((char*)val, sizeof(val))) {
            cur = (uint64_t)ntohl(val[0]) << 32 | (uint64_t)ntohl(val[1]);
            cqf.insertValue(cur);
            printf("\ninserting: %lu\n", cur);
        }
    }
    for (uint64_t j = 0; j < 1; j++) {
        printf("\nQuerying %lu: %d\n",j, cqf.query((uint64_t) 16));
        printf("\nQuerying %lu: %d\n",j, cqf.query((uint64_t) 4773016448775093842));
        printf("\n");
    }*/

    delete[] cqf.qf;
    idt.close();
}