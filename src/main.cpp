#include <cstdio>
#include <cstring>
#include "qap_instance.h"
#include "experiment.h"

static const char* INSTANCE_NAMES[] = {
    "chr12a",
    "chr15a",
    "nug12",
    "nug15",
    "tai64c",
    "tai12a",
    "tai20a",
    // "tai256c",  
    "sko81",
};
static const int N_INSTANCES = 8;

int main() {
    // Open CSV outputs
    FILE* csv_main = fopen("../results/results.csv", "w");
    FILE* csv_scatter = fopen("../results/scatter_results.csv", "w");
    FILE* csv_sim = fopen("../results/similarity_results.csv", "w");

    if (csv_main) write_csv_header(csv_main);
    if (csv_scatter) write_scatter_header(csv_scatter);
    if (csv_sim) write_similarity_header(csv_sim);

    for (int k = 0; k < N_INSTANCES; k++) {
        const char* name = INSTANCE_NAMES[k];

        char dat_path[256], sln_path[256];
        snprintf(dat_path, sizeof(dat_path), "../instances/%s.dat", name);
        snprintf(sln_path, sizeof(sln_path), "../instances/%s.sln", name);

        QAPInstance inst;
        memset(&inst, 0, sizeof(inst));
        strncpy(inst.name, name, sizeof(inst.name) - 1);

        if (!load_dat(dat_path, inst)) {
            fprintf(stderr, "Skipping %s (dat failed)\n", name); continue;
        }
        if (!load_sln(sln_path, inst)) {
            fprintf(stderr, "Skipping %s (sln failed)\n", name);
            free_instance(inst); continue;
        }
        if (!check_symmetry(inst)) {
            fprintf(stderr, "Skipping %s (asymmetric matrices)\n", name);
            free_instance(inst); continue;
        }

        // Verify optimal cost interpretation
        long long check = 0;
        for (int i = 0; i < inst.n; i++)
            for (int j = 0; j < inst.n; j++)
                check += (long long)inst.F(i, j)
                       * (long long)inst.D(inst.opt_perm[i], inst.opt_perm[j]);

        printf("\n=== %s (n=%d, opt=%lld) [verify: %s] ===\n",
               name, inst.n, inst.opt_cost,
               (check == inst.opt_cost) ? "OK" : "MISMATCH");

        // Use instance index as base seed for reproducibility
        uint64_t base_seed = 0xABCDEF01ULL + (uint64_t)k * 999983ULL;
        
        printf("  Running main experiment (10 runs)...\n");
        for (int run = 0; run < 10; run++) {
            uint64_t run_seed = base_seed + run * 12345ULL;
            ExperimentResults res = run_experiment(inst, run_seed);

            // Print only the first run to keep the console readable
            if (run == 0) {
                printf("  [Run 1] time budget: %.2f ms\n", res.budget_ms);
                // 2. Print Local Search results
                print_run_result(name, "G",   inst.opt_cost, res.greedy);
                print_run_result(name, "S",   inst.opt_cost, res.steepest);
                print_run_result(name, "3O",  inst.opt_cost, res.opt3);

                // 3. Print Metaheuristic and Random results
                print_run_result(name, "VNS", inst.opt_cost, res.vns);
                print_run_result(name, "RS",  inst.opt_cost, res.rs);
                print_run_result(name, "RW",  inst.opt_cost, res.rw);

                // 4. Print the Heuristic construction result
                print_run_result(name, "H",   inst.opt_cost, res.heuristic);
                
            }

            // Write all 10 runs to the CSV for statistical evaluation
            if (csv_main) {
                write_csv_rows(csv_main, name, run, "G",  inst.opt_cost, res.greedy);
                write_csv_rows(csv_main, name, run, "S",  inst.opt_cost, res.steepest);
                write_csv_rows(csv_main, name, run, "RS", inst.opt_cost, res.rs);
                write_csv_rows(csv_main, name, run, "RW", inst.opt_cost, res.rw);
                write_csv_rows(csv_main, name, run, "H", inst.opt_cost, res.heuristic);
                write_csv_rows(csv_main, name, run, "3O",  inst.opt_cost, res.opt3); // New
                write_csv_rows(csv_main, name, run, "VNS", inst.opt_cost, res.vns);  // New
                write_csv_rows(csv_main, name, run, "REV", inst.opt_cost, res.reverse);    
            }
        }

        // 2. Scatter Experiment (400 runs)
        printf("  Running scatter experiment (400 runs)...\n");
        if (csv_scatter) run_scatter_experiment(inst, 400, base_seed + 1, csv_scatter);

        // 3. Similarity Experiment (100 runs)
        printf("  Running similarity experiment (100 runs)...\n");
        if (csv_sim) run_similarity_experiment(inst, 100, base_seed + 2, csv_sim);

        free_instance(inst);
    }

    if (csv_main) fclose(csv_main);
    if (csv_scatter) fclose(csv_scatter);
    if (csv_sim) fclose(csv_sim);
    
    printf("\nAll experiments finished!\n");
    return 0;
}