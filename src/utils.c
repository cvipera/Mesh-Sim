#include "../lib/utils.h"

uint64_t bitmask[WORD_COUNT];

void bit_set(uint64_t *bm, int i) {
    bm[i / WORD_BITS] |= (1ULL << (i % WORD_BITS));
}

void bit_clear(uint64_t *bm, int i) {
    bm[i / WORD_BITS] &= ~(1ULL << (i % WORD_BITS));
}

int bit_test(const uint64_t *bm, int i) {
    return (bm[i / WORD_BITS] >> (i % WORD_BITS)) & 1ULL;
}