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
        res.steps++; // Used here to track total solutions generated
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