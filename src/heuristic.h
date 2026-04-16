#pragma once
#include "qap_instance.h"
#include "solution.h"
#include "rng.h"

// Greedy construction: assign facility with highest total flow to the
// location with highest total distance (largest-to-largest matching).
// Ties are broken randomly.
inline void greedy_construct(Solution& sol, const QAPInstance& inst, RNG& rng) {
    const int n = inst.n;

    // Compute total flow per facility and total distance per location
    int* flow_sum = (int*)malloc(n * sizeof(int));
    int* dist_sum = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        long long fs = 0, ds = 0;
        for (int j = 0; j < n; j++) { fs += inst.F(i, j); ds += inst.D(i, j); }
        flow_sum[i] = (int)fs;
        dist_sum[i] = (int)ds;
    }

    // Sort facility indices by descending flow
    int* fac_order = (int*)malloc(n * sizeof(int));
    int* loc_order = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) { fac_order[i] = i; loc_order[i] = i; }

    // Simple insertion sort (n is small enough)
    for (int i = 1; i < n; i++) {
        int fi = fac_order[i], li = loc_order[i];
        int j = i;
        while (j > 0 && flow_sum[fac_order[j-1]] < flow_sum[fi]) {
            fac_order[j] = fac_order[j-1]; j--;
        }
        fac_order[j] = fi;
        j = i;
        while (j > 0 && dist_sum[loc_order[j-1]] < dist_sum[li]) {
            loc_order[j] = loc_order[j-1]; j--;
        }
        loc_order[j] = li;
    }

    // Assign: facility fac_order[i] → location loc_order[i]
    for (int i = 0; i < n; i++)
        sol.perm[fac_order[i]] = loc_order[i];

    sol.cost = full_cost(inst, sol.perm);

    free(flow_sum); free(dist_sum); free(fac_order); free(loc_order);
}