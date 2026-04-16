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

print("All plots generated successfully in ../results/")