#include <filterUtils.hpp>
#define MEM_UNIT 8

int asm_rank(uint64_t val, int pos)
{
    // Keep only bits up to pos pos non-including.
    val = val & (((2ULL) << pos) - 1);

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int asm_select(uint64_t val, int n)
{          
    uint64_t pos = 1ULL << n;

    asm("pdep %[val], %[mask], %[val]"
		: [val] "+r" (val)
		: [mask] "r" (pos));
	
    asm("tzcnt %[bit], %[index]"
        : [index] "=r" (pos)
        : [bit] "g" (val)
		: "cc");

    return (pos);
}

using namespace std;
void set8(uint8_t * pos, uint8_t value, uint8_t mask)
{
    *pos = (*pos & ~mask);
    *pos += (value & mask);
}

void printbits_rev(uint8_t * mem, size_t offset, size_t num_bits)
{
    uint8_t current_byte = (*mem) >> offset;
    size_t remaining_bits = MEM_UNIT - offset;

    uint i;
    for (i = 0; i < num_bits; i++) {
        cout << ((current_byte & 0b1) ? '1' : '0'); //masking rightmost bit and evaluating if 1 or 0
        current_byte >>= 1;
        remaining_bits -= 1;

        if (remaining_bits == 0) {
            mem += 1;
            current_byte = *mem;
            remaining_bits = MEM_UNIT;
        }
    }
    cout << endl;
}

void print_bits(uint8_t * mem, size_t offset, size_t num_bits)
{
    size_t remaining_bits = MEM_UNIT - offset;
    uint8_t current_byte = (*mem) << offset;

    cout << (uint64_t *)mem << endl;

    uint i;
    for (i = 0; i < num_bits; i++) {
        cout << ((current_byte & 0b10000000) ? '1' : '0');
        current_byte <<= 1;
        remaining_bits -= 1;
        if (remaining_bits == 0) {
            mem += 1;
            current_byte = *mem;
            remaining_bits = MEM_UNIT;

            if ((num_bits - i) < MEM_UNIT)
                remaining_bits -= (num_bits - i);
        }
    }
    cout << endl << i << endl;
}

void set_bits_8(uint8_t * mem, uint8_t bits, uint8_t mask)
{
    uint8_t togive_bits = (bits & mask);
    *mem = togive_bits;
}


uint8_t get_mask_right(int num_bits_to_set)
{
    uint8_t mask = 0b1;
    mask <<= num_bits_to_set;
    mask -= 1;
    return mask;
}

uint8_t getmaskleft(int num_bits_to_set)
{
    uint8_t mask = 0b1;
    mask <<= (MEM_UNIT-num_bits_to_set);
    mask -=1;

    return ~mask;
}
/*
void set8bits(uint8_t * mem, size_t offset, uint8_t bits) {
    uint8_t current_byte = (*mem) >> offset;
    size_t remaining_bits = MEM_UNIT - offset;

    uint i;
    for (i=0 ; i<num_bits ; i++) {
        cout << ((current_byte & 0b1) ? '1' : '0');
        current_byte >>= 1;
        remaining_bits -= 1;

        if (remaining_bits == 0) {
            mem += 1;
            current_byte = *mem;
            remaining_bits = MEM_UNIT;
        }
    }
    cout << endl;
}
*/

/*
 void printbits(unsigned long number, unsigned int num_bits_to_print)
 {
      if (num_bits_to_print > 0) {
          printbits(number >> 1, num_bits_to_print - 1);
          printf("%d", (int) number & 1);
      }
      if (num_bits_to_print == 0)
          printf("\n");
 }
*/

inline uint8_t getNthBitFrom(uint64_t vec, int n) {
    return (vec >> n) & 0b1;
}

inline void setNthBitFrom(uint64_t &vec, int n) {
    vec |= 1ULL << n;
}

inline void setNthBitToX(uint64_t &vec, int n, int x) {
    vec ^= (-x ^ vec) & (1ULL << n);
}

inline void clearNthBitFrom(uint64_t &vec, int n) {
    vec &= ~(1ULL << n);
}
