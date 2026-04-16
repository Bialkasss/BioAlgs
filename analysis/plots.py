import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# 1. Load the data
df = pd.read_csv('../results/results.csv')

# 2. Pre-processing & Feature Engineering
# Calculate Quality Improvement for Efficiency
df['Improvement'] = df['InitCost'] - df['FinalCost']
# Efficiency = Improvement / Time (Handling division by zero just in case)
df['Efficiency'] = df['Improvement'] / df['TimeMS'].replace(0, 1)
# Dictionary of known optimal (or best known) values for these standard instances.
# (Note: Please verify these match the exact dataset variations you are using)
optima_map = {
    "chr12a": 9552, 
    "nug12": 578, 
    "tai12a": 224416, 
    "chr15a": 9896, 
    "nug15": 1150, 
    "tai20a": 703482, 
    "tai64c": 1855928, 
    "sko81": 90998, 
    "tai256c": 44759294
}

# Map optimal values to the dataframe
df['Optimum'] = df['Instance'].map(optima_map)

# Calculate Quality: (Cost - Optimum) / Optimum
# (A Quality of 0.0 means the absolute optimum was found)
df['Quality'] = (df['FinalCost'] - df['Optimum']) / df['Optimum']
# Map instances to sizes for sorting
n_map = {
    "chr12a": 12, "nug12": 12, "tai12a": 12, 
    "chr15a": 15, "nug15": 15, "tai20a": 20, 
    "tai64c": 64, "sko81": 81, "tai256c": 256
}
df['n'] = df['Instance'].map(n_map)
df = df.sort_values(by='n')

# Set aesthetic style
sns.set_style("whitegrid")
palette = 'bright'

# --- PLOT 1: RUNTIME ---
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x='Instance', y='TimeMS', hue='Algo', marker='o', errorbar='sd', palette=palette)
# sns.lineplot(
#     data=df, 
#     x='Instance', 
#     y='Gap', 
#     hue='Algo', 
#     palette='bright', # This will automatically assign new colors
#     errorbar='sd'
# )
plt.title('Algorithm Runtime vs. Instance Size', fontsize=14)
plt.ylabel('Runtime (ms)')
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('../results/plot_runtime.png')

# --- PLOT 2: EFFICIENCY ---
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x='Instance', y='Efficiency', hue='Algo', marker='o', errorbar='sd', palette=palette)
plt.title('Search Efficiency (Cost Improvement / ms)', fontsize=14)
plt.ylabel('Efficiency')
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('../results/plot_efficiency.png')

# --- PLOT 3: ALGORITHM STEPS (G & S Only) ---
plt.figure(figsize=(10, 6))
gs_df = df[df['Algo'].isin(['G', 'S'])]
sns.lineplot(data=gs_df, x='Instance', y='Steps', hue='Algo', marker='o', errorbar='sd', palette=['blue', 'red'])
plt.title('Number of Improving Steps (Local Search Only)', fontsize=14)
plt.ylabel('Steps')
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('../results/plot_steps.png')

# --- PLOT 4: SOLUTION EVALUATIONS ---
plt.figure(figsize=(10, 6))
sns.lineplot(data=df, x='Instance', y='Evals', hue='Algo', marker='o', errorbar='sd', palette=palette)
plt.title('Total Delta Evaluations (Search Effort)', fontsize=14)
plt.ylabel('Number of Evaluations')
plt.yscale('log') # Log scale is often better here as Evals grow very fast with N
plt.xticks(rotation=45)
plt.tight_layout()
plt.savefig('../results/plot_evals.png')
# --- PLOT 5a: SOLUTION QUALITY (ALL ALGORITHMS) ---
plt.figure(figsize=(10, 6))

# Using seaborn's lineplot natively calculates the mean and creates the "shadow" (standard deviation)
sns.lineplot(
    data=df, 
    x='Instance', 
    y='Quality', 
    hue='Algo', 
    marker='o', 
    errorbar='sd', 
    palette=palette
)

plt.title('Solution Quality: Distance from Optimum (All Methods)', fontsize=14)
plt.ylabel('Quality ((Cost - Opt) / Opt)\n<-- Lower is better')
plt.xticks(rotation=45)

# Place legend outside the graph to prevent it from covering the lines/shadows
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.tight_layout()
plt.savefig('../results/plot_quality_all.png')


# --- PLOT 5b: SOLUTION QUALITY (RS, RW, G, S, H ONLY) ---
plt.figure(figsize=(10, 6))

# Filter the dataframe for the requested subset of algorithms
target_algos = ['RS', 'RW', 'G', 'S', 'H']
df_subset = df[df['Algo'].isin(target_algos)]

sns.lineplot(
    data=df_subset, 
    x='Instance', 
    y='Quality', 
    hue='Algo', 
    marker='o', 
    errorbar='sd', 
    palette=palette # Re-uses your 'bright' palette
)

plt.title('Solution Quality: Distance from Optimum (Subset)', fontsize=14)
plt.ylabel('Quality ((Cost - Opt) / Opt)\n<-- Lower is better')
plt.xticks(rotation=45)

# Place legend outside
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.tight_layout()
plt.savefig('../results/plot_quality_subset.png')
print("All plots generated successfully in ../results/")