#include <filterUtils.hpp>

void printbits(unsigned long number, unsigned int num_bits_to_print)
{
    if (num_bits_to_print > 0) {
        printbits(number >> 1, num_bits_to_print - 1);
        printf("%d", (int) number & 1);
    }
}

void printbits64(unsigned long number) {
    printbits(number, 64);
}

void printbits8(unsigned long number) {
    printbits(number, 8);
}

int asmRank(uint64_t val, int pos)
{
    // Keep only bits up to pos pos non-including.
    val = val & (((2ULL) << pos) - 1);

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int asmSelect(uint64_t val, int n)
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
