#include "algorithms.h"
#include "neighbourhood.h"
#include "heuristic.h"
#include <chrono>
#include <algorithm>

// Helper to get elapsed time in milliseconds
static double get_time_ms(std::chrono::high_resolution_clock::time_point start) {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(now - start).count();
}

RunResult run_greedy(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    init_random(current, inst, rng);
    res.init_cost = current.cost;

    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < inst.n - 1 && !improved; i++) {
            for (int j = i + 1; j < inst.n && !improved; j++) {
                res.evals++;
                long long delta = swap_delta(inst, current.perm, i, j);
                if (delta < 0) {
                    apply_swap(current, i, j, delta);
                    res.steps++;
                    improved = true;
                }
            }
        }
    }

    best.copy_from(current);
    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}

RunResult run_steepest(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    init_random(current, inst, rng);
    res.init_cost = current.cost;

    bool improved = true;
    while (improved) {
        improved = false;
        long long best_delta = 0;
        int best_i = -1, best_j = -1;

        for (int i = 0; i < inst.n - 1; i++) {
            for (int j = i + 1; j < inst.n; j++) {
                res.evals++;
                long long delta = swap_delta(inst, current.perm, i, j);
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (best_delta < 0) {
            apply_swap(current, best_i, best_j, best_delta);
            res.steps++;
            improved = true;
        }
    }

    best.copy_from(current);
    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}

RunResult run_random_search(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng, double budget_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    init_random(best, inst, rng);
    res.init_cost = best.cost;
    res.evals++;

    while (get_time_ms(start) < budget_ms) {
        init_random(current, inst, rng);
        res.evals++;
        
        if (current.cost < best.cost) {
            best.copy_from(current);
        }
        res.steps++; // Used to track total solutions generated
    }

    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}

RunResult run_random_walk(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng, double budget_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    init_random(current, inst, rng);
    best.copy_from(current);
    res.init_cost = current.cost;

    while (get_time_ms(start) < budget_ms) {
        int i = rng_range(rng, 0, inst.n);
        int j = rng_range(rng, 0, inst.n);
        while (i == j) {
            j = rng_range(rng, 0, inst.n);
        }
        
        res.evals++;
        long long delta = swap_delta(inst, current.perm, i, j);
        apply_swap(current, i, j, delta); // Unconditional swap

        if (current.cost < best.cost) {
            best.copy_from(current);
        }
        res.steps++; // Track number of steps taken
    }

    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}

RunResult run_heuristic(const QAPInstance& inst, Solution& out, RNG& rng) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    greedy_construct(out, inst, rng);
    res.init_cost = out.cost;
    res.cost = out.cost;
    res.evals = 1;

    res.time_ms = get_time_ms(start);
    return res;
}

RunResult run_3opt_greedy(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};
    init_random(current, inst, rng);
    res.init_cost = current.cost;

    bool improved = true;
    while (improved) {
        improved = false;
        // O(n^3) neighborhood scan
        for (int i = 0; i < inst.n - 2 && !improved; i++) {
            for (int j = i + 1; j < inst.n - 1 && !improved; j++) {
                for (int k = j + 1; k < inst.n && !improved; k++) {
                    
                    // Rotation A: (i->j, j->k, k->i)
                    // This is equivalent to swap(i,j) then swap(j,k)
                    long long d1 = swap_delta(inst, current.perm, i, j);
                    apply_swap(current, i, j, d1); // Temporary apply
                    long long d2 = swap_delta(inst, current.perm, j, k);
                    
                    if (d1 + d2 < 0) {
                        apply_swap(current, j, k, d2);
                        res.evals += 2; res.steps++; improved = true;
                    } else {
                        apply_swap(current, i, j, -d1); // Revert
                        res.evals += 2;
                        
                        // Rotation B: (i->k, k->j, j->i)
                        // Equivalent to swap(i,k) then swap(k,j)
                        long long d3 = swap_delta(inst, current.perm, i, k);
                        apply_swap(current, i, k, d3);
                        long long d4 = swap_delta(inst, current.perm, k, j);
                        
                        if (d3 + d4 < 0) {
                            apply_swap(current, k, j, d4);
                            res.evals += 2; res.steps++; improved = true;
                        } else {
                            apply_swap(current, i, k, -d3); // Revert
                            res.evals += 2;
                        }
                    }
                }
            }
        }
    }
    best.copy_from(current);
    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}

// Helper: Shaking (k random swaps)
void shake(Solution& sol, const QAPInstance& inst, int k, RNG& rng) {
    for (int i = 0; i < k; i++) {
        int r1 = rng_range(rng, 0, inst.n);
        int r2 = rng_range(rng, 0, inst.n);
        while (r1 == r2) r2 = rng_range(rng, 0, inst.n);
        
        long long delta = swap_delta(inst, sol.perm, r1, r2);
        apply_swap(sol, r1, r2, delta);
    }
}

RunResult run_vns(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng, double budget_ms) {
    auto start = std::chrono::high_resolution_clock::now();
    RunResult res = {0};

    // Initial solution via Greedy 2-opt
    res = run_greedy(inst, current, best, rng);
    res.init_cost = res.cost;
    
    int k = 1;
    const int k_max = (inst.n > 10) ? 5 : 2; // Max "shake" distance

    while (get_time_ms(start) < budget_ms) {
        // 1. Shaking: Move to a random solution in neighborhood Nk(best)
        current.copy_from(best);
        shake(current, inst, k, rng);

        // 2. Local Search (Greedy 2-opt)
        bool improved_inner = true;
        while (improved_inner) {
            improved_inner = false;
            for (int i = 0; i < inst.n - 1 && !improved_inner; i++) {
                for (int j = i + 1; j < inst.n && !improved_inner; j++) {
                    res.evals++;
                    long long delta = swap_delta(inst, current.perm, i, j);
                    if (delta < 0) {
                        apply_swap(current, i, j, delta);
                        improved_inner = true;
                    }
                }
            }
        }

        // 3. Neighborhood Change
        if (current.cost < best.cost) {
            best.copy_from(current);
            k = 1; // Success: Reset to smallest neighborhood
            res.steps++;
        } else {
            k++; // Failure: Try a larger jump
            if (k > k_max) k = 1;
        }
    }

    res.cost = best.cost;
    res.time_ms = get_time_ms(start);
    return res;
}
