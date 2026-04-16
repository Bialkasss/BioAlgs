#pragma once
#include <random>
#include <algorithm> // For std::shuffle

using RNG = std::mt19937;

inline RNG make_rng(uint64_t seed) { return RNG(seed); }

inline void shuffle_perm(int* perm, int n, RNG& rng) {
    std::shuffle(perm, perm + n, rng);
}

inline int rng_range(RNG& rng, int lo, int hi) {   // [lo, hi)
    return std::uniform_int_distribution<int>(lo, hi - 1)(rng);
}