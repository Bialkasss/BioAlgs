#pragma once
#include "qap_instance.h"
#include "solution.h"

// Compute the cost delta of swapping facilities i and j in perm[].
// Uses the standard O(n) QAP swap-delta formula.
// delta > 0 means the swap makes things worse; delta < 0 means improvement.
inline long long swap_delta(const QAPInstance& inst, const int* perm, int r, int s) {
    const int n = inst.n;
    long long delta = 0;

    // Contribution of the (r,s) pair itself cancels — loop over all k != r, s
    for (int k = 0; k < n; k++) {
        if (k == r || k == s) continue;
        delta += (long long)(inst.F(r, k) - inst.F(s, k))
               * (long long)(inst.D(perm[s], perm[k]) - inst.D(perm[r], perm[k]));
        delta += (long long)(inst.F(k, r) - inst.F(k, s))
               * (long long)(inst.D(perm[k], perm[s]) - inst.D(perm[k], perm[r]));
    }
   
    return delta;
}

// Apply swap of facilities i and j in-place, update cost
inline void apply_swap(Solution& sol, int i, int j, long long delta) {
    int tmp = sol.perm[i];
    sol.perm[i] = sol.perm[j];
    sol.perm[j] = tmp;
    sol.cost += delta;
}

// Number of swap pairs in an n-element permutation
inline int neighborhood_size(int n) {
    return n * (n - 1) / 2;
}