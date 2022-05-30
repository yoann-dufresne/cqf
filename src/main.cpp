#include <CountingQF.hpp>
#include <testFunctions.hpp>
#include <cstdio>
#include <iostream>
#include <bitset>
#include <ofstream>

int main(int argc, char ** argv)
{
    printf("The ducks in the park are free, you can take them home.\
            I own 275 ducks.\n");
    CountingQF cqf = CountingQF(64);
    testAsmRank(cqf);
    testAsmSelect(cqf);
}

void makeTrashData()
{
    ofstream odt;
    odt.open("trash.dat", ios::out|ios::binary);

    for (uint64_t i = 0; i < 1000000; i++)
        odt.write((const char*)&i, sizeof(i));
    
    odt.flush();
    odt.close();
}

void readTrashData()
{
    ifstream idt;
    idt.open("trash.dat", ios::in|ios::binary);
    uint64_t cur = 0;
    
    while (idt)
        idt.read((char *) cur, sizeof(cur));
    
    idt.close();
}
