#pragma once
#include <cstdlib>
#include <cstring>
#include "qap_instance.h"
#include "rng.h"

struct Solution {
    int* perm;       // [n] permutation: perm[facility] = location
    long long cost;
    int n;

    void alloc(int _n) {
        n = _n;
        perm = (int*)malloc(n * sizeof(int));
        cost = -1;
    }

    void free_mem() {
        free(perm);
        perm = nullptr;
    }

    void copy_from(const Solution& other) {
        memcpy(perm, other.perm, n * sizeof(int));
        cost = other.cost;
    }
};

// Compute full O(n^2) cost of a solution
inline long long full_cost(const QAPInstance& inst, const int* perm) {
    long long c = 0;
    for (int i = 0; i < inst.n; i++)
        for (int j = 0; j < inst.n; j++)
            c += (long long)inst.F(i, j) * (long long)inst.D(perm[i], perm[j]);
    return c;
}

// Initialize solution as identity permutation [0, 1, ..., n-1]
inline void init_identity(Solution& sol, const QAPInstance& inst) {
    for (int i = 0; i < inst.n; i++) sol.perm[i] = i;
    sol.cost = full_cost(inst, sol.perm);
}

// Initialize solution with a random permutation
inline void init_random(Solution& sol, const QAPInstance& inst, RNG& rng) {
    for (int i = 0; i < inst.n; i++) sol.perm[i] = i;
    shuffle_perm(sol.perm, inst.n, rng);
    sol.cost = full_cost(inst, sol.perm);
}