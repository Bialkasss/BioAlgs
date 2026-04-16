#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct QAPInstance {
    int  n;
    int* dist;       // [n*n] row-major  (distances)
    int* flow;       // [n*n] row-major  (flows)
    long long opt_cost;
    int* opt_perm;   // [n] 0-based optimal permutation, filled by load_sln
    char name[64];

    // Accessors
    inline int D(int i, int j) const { return dist[i * n + j]; }
    inline int F(int i, int j) const { return flow[i * n + j]; }
};

// Call after n is known — allocates dist, flow, opt_perm
inline void alloc_instance(QAPInstance& inst, int n) {
    inst.n        = n;
    inst.dist     = (int*)malloc(n * n * sizeof(int));
    inst.flow     = (int*)malloc(n * n * sizeof(int));
    inst.opt_perm = (int*)malloc(n * sizeof(int));
    inst.opt_cost = -1;
}

inline void free_instance(QAPInstance& inst) {
    free(inst.dist);
    free(inst.flow);
    free(inst.opt_perm);
    inst.dist = inst.flow = inst.opt_perm = nullptr;
}

// Loads .dat file — reads n, then dist matrix, then flow matrix
// Returns true on success
inline bool load_dat(const char* path, QAPInstance& inst) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "ERROR: cannot open dat file: %s\n", path);
        return false;
    }

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        fprintf(stderr, "ERROR: cannot read n from: %s\n", path);
        fclose(f);
        return false;
    }

    alloc_instance(inst, n);

    

    // flow matrix
    for (int i = 0; i < n * n; i++) {
        if (fscanf(f, "%d", &inst.flow[i]) != 1) {
            fprintf(stderr, "ERROR: flow matrix read failed at element %d\n", i);
            fclose(f);
            return false;
        }
    }
    // distance matrix
    for (int i = 0; i < n * n; i++) {
        if (fscanf(f, "%d", &inst.dist[i]) != 1) {
            fprintf(stderr, "ERROR: dist matrix read failed at element %d\n", i);
            fclose(f);
            return false;
        }
    }

    fclose(f);
    return true;
}

// Loads .sln file — reads n, opt_cost, then permutation (converts 1-based -> 0-based)
// Returns true on success
inline bool load_sln(const char* path, QAPInstance& inst) {
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "ERROR: cannot open sln file: %s\n", path);
        return false;
    }

    int n;
    long long opt_cost;
    if (fscanf(f, "%d %lld", &n, &opt_cost) != 2) {
        fprintf(stderr, "ERROR: cannot read header from: %s\n", path);
        fclose(f);
        return false;
    }

    if (n != inst.n) {
        fprintf(stderr, "ERROR: sln n=%d does not match dat n=%d\n", n, inst.n);
        fclose(f);
        return false;
    }

    inst.opt_cost = opt_cost;

    for (int i = 0; i < n; i++) {
        int val;
        if (fscanf(f, "%d", &val) != 1) {
            fprintf(stderr, "ERROR: perm read failed at position %d\n", i);
            fclose(f);
            return false;
        }
        inst.opt_perm[i] = val -1 ; 
    }

    fclose(f);
    return true;
}

inline bool check_symmetry(const QAPInstance& inst) {
    for (int i = 0; i < inst.n; i++) {
        for (int j = 0; j < inst.n; j++) {
            if (inst.D(i, j) != inst.D(j, i)) {
                fprintf(stderr, "ERROR [%s]: dist matrix not symmetric at (%d,%d): %d != %d\n",
                        inst.name, i, j, inst.D(i,j), inst.D(j,i));
                return false;
            }
            if (inst.F(i, j) != inst.F(j, i)) {
                fprintf(stderr, "ERROR [%s]: flow matrix not symmetric at (%d,%d): %d != %d\n",
                        inst.name, i, j, inst.F(i,j), inst.F(j,i));
                return false;
            }
        }
    }
    return true;
}


// Debug print — call to verify loading
inline void print_instance(const QAPInstance& inst) {
    printf("=== Instance: %s ===\n", inst.name);
    printf("n = %d\n", inst.n);
    printf("opt_cost = %lld\n", inst.opt_cost);

    printf("dist matrix:\n");
    for (int i = 0; i < inst.n; i++) {
        for (int j = 0; j < inst.n; j++)
            printf("%4d", inst.D(i, j));
        printf("\n");
    }

    printf("flow matrix:\n");
    for (int i = 0; i < inst.n; i++) {
        for (int j = 0; j < inst.n; j++)
            printf("%4d", inst.F(i, j));
        printf("\n");
    }

    printf("opt_perm (0-based): ");
    for (int i = 0; i < inst.n; i++)
        printf("%d ", inst.opt_perm[i]);
    printf("\n");
}