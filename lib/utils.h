#pragma once 

#include <stdint.h>

#define BITCOUNT 150000
#define WORD_BITS 64
#define WORD_COUNT (BITCOUNT / WORD_BITS)


extern uint64_t bitmask[WORD_COUNT];

/// @brief Set the i-th bit in the bitmask
/// @param bm The bitmask array
/// @param i The bit index to set
void bit_set(uint64_t *bm, int i);

/// @brief Clear the i-th bit in the bitmask
/// @param bm The bitmask array
/// @param i The bit index to clear
void bit_clear(uint64_t *bm, int i);

/// @brief Test if the i-th bit in the bitmask is set
/// @param bm The bitmask array
/// @param i The bit index to test
/// @return 1 if the bit is set, 0 otherwise
int bit_test(const uint64_t *bm, int i);