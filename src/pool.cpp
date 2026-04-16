#include "pool.h"
#include "solution.h"
// #include "qap_instance.h"
#include <cstdlib>

void alloc_pool(SolutionPool& p, int n, int capacity) {
    p.n = n;
    p.capacity = capacity;
    p.data = (int*)malloc(capacity * n * sizeof(int));
}

void free_pool(SolutionPool& p) {
    free(p.data);
    p.data = nullptr;
    p.n = 0;
    p.capacity = 0;
}

void pool_get(const SolutionPool& p, int k, Solution& out) {
    out.n = p.n;
    out.perm = p.data + (k * p.n);
    out.cost = -1; // Ready to be initialized
}