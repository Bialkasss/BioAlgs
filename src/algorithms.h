#pragma once
#include "qap_instance.h"
#include "solution.h"
#include "rng.h"

struct RunResult {
    long long cost;       // final solution cost
    long long init_cost;  // initial solution cost (for scatter plot)
    double time_ms;
    long long steps;      // only G/S: number of improving moves
    long long evals;      // all: number of delta evaluations
};

// Local search — single run from random start to local optimum
// algorithms.h
// sol_current = slot 0, sol_best = slot 1 — caller binds them from pool
RunResult run_greedy  (const QAPInstance& inst, Solution& current,
                       Solution& best, RNG& rng);
RunResult run_steepest(const QAPInstance& inst, Solution& current,
                       Solution& best, RNG& rng);
// Time-capped random algorithms — stop after budget_ms milliseconds

RunResult run_random_search(const QAPInstance& inst, Solution& current,
                            Solution& best, RNG& rng, double budget_ms);
RunResult run_random_walk  (const QAPInstance& inst, Solution& current,
                            Solution& best, RNG& rng, double budget_ms);

// Non-deterministic heuristic — greedy construction with ties broken randomly
RunResult run_heuristic    (const QAPInstance& inst, Solution& out,
                            RNG& rng);

// ADd other neigh
RunResult run_3opt_greedy(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng);
RunResult run_vns(const QAPInstance& inst, Solution& current, Solution& best, RNG& rng, double budget_ms);