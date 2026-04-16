import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# 1. Load the scatter experiment data
# Expected columns: Instance, Run, Algo, InitCost, FinalCost
df = pd.read_csv('../results/scatter_results.csv')

# 2. Setup targets and plot parameters
instances = ['sko81', 'tai64c']
max_restarts = 300
sns.set_style("whitegrid")

for inst in instances:
    plt.figure(figsize=(12, 7))
    
    # Filter for the specific instance and limit to the first 300 restarts
    inst_df = df[(df['Instance'] == inst) & (df['Run'] < max_restarts)].copy()
    
    # Process each algorithm (Greedy and Steepest)
    for algo, color in zip(['G', 'S'], ['royalblue', 'crimson']):
        # Ensure data is sorted by the run index
        algo_df = inst_df[inst_df['Algo'] == algo].sort_values('Run')
        
        # Calculate 'Best So Far' (cumulative minimum)
        algo_df['BestSoFar'] = algo_df['FinalCost'].cummin()
        
        # Calculate 'Average So Far' (cumulative mean)
        algo_df['AvgSoFar'] = algo_df['FinalCost'].expanding().mean()
        
        # Plot Best So Far (solid line)
        plt.plot(algo_df['Run'], algo_df['BestSoFar'], 
                 label=f'{algo} - Best so far', 
                 color=color, linewidth=2.5)
        
        # Plot Average So Far (dashed line)
        plt.plot(algo_df['Run'], algo_df['AvgSoFar'], 
                 label=f'{algo} - Average so far', 
                 color=color, linestyle='--', alpha=0.6)

    # 3. Final Formatting
    plt.title(f'Multi-Random Start (MRS) Analysis: {inst}', fontsize=16)
    plt.xlabel('Number of Restarts', fontsize=12)
    plt.ylabel('Solution Cost', fontsize=12)
    plt.legend(loc='upper right', frameon=True, shadow=True)
    
    # Focus the Y-axis on the range of results to make improvements visible
    y_min = inst_df['FinalCost'].min() * 0.99
    y_max = inst_df['FinalCost'].max() * 1.01
    plt.ylim(y_min, y_max)
    
    plt.tight_layout()
    plt.savefig(f'../results/mrs_analysis_{inst}.png', dpi=300)
    plt.close()

print("MRS Analysis plots generated successfully.")