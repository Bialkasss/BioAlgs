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
// Reverses the segment of the permutation between indices i and j (inclusive)
inline void apply_reverse(Solution& sol, int i, int j, long long delta) {
    while (i < j) {
        int tmp = sol.perm[i];
        sol.perm[i] = sol.perm[j];
        sol.perm[j] = tmp;
        i++; j--;
    }
    sol.cost += delta;
}

// Calculate delta for reversing segment [start, end]
// O(k * n) complexity where k = end - start + 1
inline long long reverse_delta(const QAPInstance& inst, const int* perm, int start, int end) {
    long long delta = 0;
    int n = inst.n;
    
    // Create a temporary copy of the affected range to find new positions
    // In a high-perf solver, you'd use a stack buffer or pool slot here.
    for (int a = start; a <= end; ++a) {
        int old_loc = perm[a];
        int new_loc = perm[end - (a - start)];
        
        // Change in contribution of facility 'a' to all other facilities 'b'
        for (int b = 0; b < n; ++b) {
            if (a == b) continue;
            int loc_b = perm[b];
            
            // If b is also in the reversed range, we must use its future position
            if (b >= start && b <= end) {
                loc_b = perm[end - (b - start)];
                // Avoid double-counting pairs within the range
                if (a > b) continue; 
            }
            
            delta += (long long)inst.F(a, b) * (inst.D(new_loc, loc_b) - inst.D(old_loc, perm[b]));
            // Add reverse flow if asymmetric (though your core is symmetric)
            delta += (long long)inst.F(b, a) * (inst.D(loc_b, new_loc) - inst.D(perm[b], old_loc));
        }
    }
    return delta;
}