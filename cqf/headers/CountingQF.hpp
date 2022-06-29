#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cmath>

/**
 * @brief Counting Quotient Filter class
 * 
 */
class CountingQF 
{   
    public:
        uint64_t numberOfSlots;
        uint64_t numberOfBlocks;
        uint64_t blockBitSize;
        uint64_t blockByteSize;

        uint64_t quotientLen;
        uint64_t remainderLen;

        uint64_t filterSize;

        uint64_t remainderPos[64][2];
        uint8_t * qf;

        explicit CountingQF(uint32_t n);
        
        /**
         * @brief Query CQF for presence.
         * 
         * @param val Check if given value is present in the CQF, 
         *            CQFs may announce false-positives.
         * @return true
         * @return false
         */
        bool query(uint64_t val);

        /**
         * @brief Insert an already hashed value into the CQF.
         * 
         * @param val Value to insert.
         */
        void insertValue(uint64_t val);

        /**
         * @brief Prints all of the CQF, blocks and remainders.
         * 
         */
        void printCQF();

        /**
         * @brief Prints block pointed to by blockAddr.
         * 
         */
        void printBlock(uint8_t * blockAddr);

        /**
         * @brief Prints a remainder that is at the address of remAddr
         * bitsToSkip bits after the first one.
         * 
         * @param remAddr Pointer to the address at the start of the remainder.
         * @param bitsToSkip Number of bits to skip at remAddr to get to the
         * start of the remainder.
         */
        void printRemainder(uint8_t * remAddr, uint8_t bitsToSkip);

         /**
         * @brief Set the remainder at the slot relative to blockAddr
         * to the value of rem.
         * 
         * @param blockAddr Block to set a value in.
         * @param slot Slot relative to the block.
         * @param rem remainder to set the value as.
         */
        void setRem(uint8_t * blockAddr, uint32_t slot, uint64_t rem);
        void setRemRev(uint8_t * blockAddr, uint32_t slot, uint64_t rem);
        uint64_t getRem(uint8_t * blockAddr, uint32_t slot);
};
#endif