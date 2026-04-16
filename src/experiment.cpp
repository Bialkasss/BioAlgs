#include "experiment.h"
#include "pool.h"
#include "rng.h"
#include "solution.h"
#include <cmath>

ExperimentResults run_experiment(const QAPInstance& inst, uint64_t base_seed) {
    ExperimentResults res = {0};

    // 1. Allocate the memory pool for exactly 2 solutions
    SolutionPool pool;
    alloc_pool(pool, inst.n, 2);

    Solution current, best;
    pool_get(pool, 0, current);
    pool_get(pool, 1, best);

    RNG rng = make_rng(base_seed);

    // 2. "Calibration" run: Time one run of Greedy
    res.greedy = run_greedy(inst, current, best, rng);
    res.budget_ms = res.greedy.time_ms;

    // Safety net for extremely small instances
    if (res.budget_ms < 1.0) res.budget_ms = 1.0; 

    // 4. Run the remaining algorithms
    res.steepest = run_steepest(inst, current, best, rng);
    res.rs = run_random_search(inst, current, best, rng, res.budget_ms);
    res.rw = run_random_walk(inst, current, best, rng, res.budget_ms);

    free_pool(pool);
    return res;
}

// UPDATE: Added 'Run' to header
void write_csv_header(FILE* csv) {
    fprintf(csv, "Instance,Run,Algo,OptCost,InitCost,FinalCost,TimeMS,Steps,Evals\n");
}

// UPDATE: Added 'run_id' parameter
void write_csv_rows(FILE* csv, const char* name, int run_id, const char* algo, long long opt_cost, const RunResult& res) {
    fprintf(csv, "%s,%d,%s,%lld,%lld,%lld,%.2f,%lld,%lld\n",
            name, run_id, algo, opt_cost, res.init_cost, res.cost, res.time_ms, res.steps, res.evals);
}

void print_run_result(const char* name, const char* algo, long long opt_cost, const RunResult& res) {
    double gap = (opt_cost > 0) ? 100.0 * (res.cost - opt_cost) / (double)opt_cost : 0.0;
    printf("  [%2s] Cost: %lld (Gap: %5.2f%%) | Time: %6.2f ms | Steps: %lld | Evals: %lld\n",
           algo, res.cost, gap, res.time_ms, res.steps, res.evals);
}

// UPDATE: Added 'Run' to scatter header
void write_scatter_header(FILE* csv) {
    fprintf(csv, "Instance,Run,Algo,InitCost,FinalCost\n");
}

void run_scatter_experiment(const QAPInstance& inst, int scatter_runs, uint64_t base_seed, FILE* csv) {
    SolutionPool pool;
    alloc_pool(pool, inst.n, 2);
    Solution current, best;
    pool_get(pool, 0, current);
    pool_get(pool, 1, best);
    RNG rng = make_rng(base_seed);

    for (int i = 0; i < scatter_runs; i++) {
        RunResult res_g = run_greedy(inst, current, best, rng);
        // UPDATE: Print the run index 'i'
        fprintf(csv, "%s,%d,G,%lld,%lld\n", inst.name, i, res_g.init_cost, res_g.cost);
        
        RunResult res_s = run_steepest(inst, current, best, rng);
        // UPDATE: Print the run index 'i'
        fprintf(csv, "%s,%d,S,%lld,%lld\n", inst.name, i, res_s.init_cost, res_s.cost);
    }
    
    free_pool(pool);
}

void write_similarity_header(FILE* csv) {
    fprintf(csv, "Instance,QualityGap,SimToOpt,SimToOther\n");
}

static double calc_similarity(const int* p1, const int* p2, int n) {
    int matches = 0;
    for (int i = 0; i < n; i++) {
        if (p1[i] == p2[i]) matches++;
    }
    return (double)matches / n;
}

void run_similarity_experiment(const QAPInstance& inst, int similarity_runs, uint64_t base_seed, FILE* csv) {
    SolutionPool pool;
    alloc_pool(pool, inst.n, similarity_runs + 2); 
    
    Solution current, best;
    pool_get(pool, similarity_runs, current);
    pool_get(pool, similarity_runs + 1, best);
    
    RNG rng = make_rng(base_seed);

    for (int i = 0; i < similarity_runs; i++) {
        Solution sol_i;
        pool_get(pool, i, sol_i);
        run_steepest(inst, current, best, rng);
        sol_i.copy_from(best); 
    }

    for (int i = 0; i < similarity_runs; i++) {
        Solution sol_i;
        pool_get(pool, i, sol_i);
        
        double quality_gap = 100.0 * (sol_i.cost - inst.opt_cost) / (double)inst.opt_cost;
        double sim_to_opt = calc_similarity(sol_i.perm, inst.opt_perm, inst.n);
        
        double sim_to_others = 0;
        for (int j = 0; j < similarity_runs; j++) {
            if (i == j) continue;
            Solution sol_j;
            pool_get(pool, j, sol_j);
            sim_to_others += calc_similarity(sol_i.perm, sol_j.perm, inst.n);
        }
        if (similarity_runs > 1) {
            sim_to_others /= (similarity_runs - 1);
        }
        
        fprintf(csv, "%s,%.4f,%.4f,%.4f\n", inst.name, quality_gap, sim_to_opt, sim_to_others);
    }
    
    free_pool(pool);
}