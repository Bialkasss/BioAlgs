#pragma once
#include "qap_instance.h"
#include "solution.h"
#include "rng.h"
#include <cstdlib>

struct SortItem {
    long long sum;
    int noise;
    int index;
};

// Comparator for qsort: Descending order by sum, then by noise for ties
inline int compare_items(const void* a, const void* b) {
    const SortItem* itemA = (const SortItem*)a;
    const SortItem* itemB = (const SortItem*)b;
    if (itemA->sum < itemB->sum) return 1;
    if (itemA->sum > itemB->sum) return -1;
    return itemB->noise - itemA->noise; // Random tie-breaker
}

inline void greedy_construct(Solution& sol, const QAPInstance& inst, RNG& rng) {
    const int n = inst.n;
    SortItem* flow_items = (SortItem*)malloc(n * sizeof(SortItem));
    SortItem* dist_items = (SortItem*)malloc(n * sizeof(SortItem));

    for (int i = 0; i < n; i++) {
        long long fs = 0, ds = 0;
        for (int j = 0; j < n; j++) { 
            fs += inst.F(i, j); 
            ds += inst.D(i, j); 
        }
        flow_items[i] = {fs, (int)rng_range(rng, 0, 1000000), i};
        dist_items[i] = {ds, (int)rng_range(rng, 0, 1000000), i};
    }

    qsort(flow_items, n, sizeof(SortItem), compare_items);
    qsort(dist_items, n, sizeof(SortItem), compare_items);

    for (int i = 0; i < n; i++) {
        sol.perm[flow_items[i].index] = dist_items[i].index;
    }

    sol.cost = full_cost(inst, sol.perm);
    free(flow_items);
    free(dist_items);
}