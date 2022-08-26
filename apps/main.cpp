#include <CountingQF.hpp>

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <ios>
#include <bit>
#include <type_traits>


#include <stdio.h>

using namespace std;

void make_homogeneous_data()
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

void make_uniform_distribution_data()
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

void endianness_check()
{
    const int value { 0x01 };
    const void * address = static_cast<const void *>(&value);
    const unsigned char * least_significant_address = static_cast<const unsigned char *>(address);
    if (*least_significant_address == 0x01)
        cout << "Little Endian" << endl;
    else if (*least_significant_address == 0x00)
        cout << "Big Endian" << endl;
    else
        cout << "Witchcraft" << endl;
}

int main(int argc, char ** argv)
{
    int cqf_size;
    endianness_check();

    if (argc != 3 && argc != 2)
    {
        printf("Usage: ./CQF <example.dat> <filterSize>\n");
        printf("    ./CQF <example.dat>\n");
        exit(1);
    }

    cqf_size = atoi(argv[2]);
    CountingQF cqf = CountingQF(cqf_size);

    std::cout << "SLOTS: " << cqf.number_of_slots << '\n';
    std::cout << "q: " << cqf.quotient_len << '\n';
    std::cout << "r: " << cqf.remainder_len << '\n';
    std::cout << "bits: " << cqf.filter_size << '\n';
    std::cout << "bytes per block: " << cqf.block_byte_size << '\n';
    std::cout << "blocks: " << cqf.number_of_blocks << '\n';


    // Code for reading values from .dat
    
    /*
    std::ifstream idt;
    idt.open(argv[1], std::ios::in|std::ios::binary);
    uint64_t cur = 0;

    while (idt) {
        uint32_t val[2] = {0};
        if (idt.read((char*)val, sizeof(val))) {
            cur = (uint64_t)ntohl(val[0]) << 32 | (uint64_t)ntohl(val[1]);
            printf("\ninserting: %lu\n", cur);
            cqf.insertValue(cur);
            printf("inserted %lu\n", cur);
        }
    
    }
    
    cqf.printCQF();
    for (uint64_t j = 0; j < 10; j++) {
        printf("\nQuerying %lu: %d",j, cqf.query(j));
    }
    idt.close();
    */
    
}