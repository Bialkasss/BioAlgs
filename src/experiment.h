#pragma once
#include "qap_instance.h"
#include "algorithms.h"
#include <cstdio>
#include <cstdint>

struct ExperimentResults {
    double budget_ms;
    RunResult greedy;
    RunResult steepest;
    RunResult rs;  // Random Search
    RunResult rw;  // Random Walk
};

// Main experiment runner
ExperimentResults run_experiment(const QAPInstance& inst, uint64_t base_seed);

// Output helpers expected by main.cpp
void write_csv_header(FILE* csv);
void write_csv_rows(FILE* csv, const char* name, int run, const char* algo, long long opt_cost, const RunResult& res);
void print_run_result(const char* name, const char* algo, long long opt_cost, const RunResult& res);

void write_scatter_header(FILE* csv);
void run_scatter_experiment(const QAPInstance& inst, int scatter_runs, uint64_t base_seed, FILE* csv);

void write_similarity_header(FILE* csv);
void run_similarity_experiment(const QAPInstance& inst, int similarity_runs, uint64_t base_seed, FILE* csv);
