#pragma once
#include "solution.h"

// pool.h
// needed for keeping all Solution objects
struct SolutionPool {
    int*  data;       // flat block: capacity * n ints
    int   n;
    int   capacity;
};

void   alloc_pool  (SolutionPool& p, int n, int capacity);
void   free_pool   (SolutionPool& p);

// Bind slot k to a Solution struct — no allocation, just pointer arithmetic
void   pool_get    (const SolutionPool& p, int k, Solution& out);