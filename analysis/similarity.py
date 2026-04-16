import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.stats import pearsonr

# 1. Load the similarity experiment data
# Ensure your C++ has been updated to include the 'Cost' column!
df = pd.read_csv('../results/similarity_results.csv')

# Instances to analyze
instances = ['sko81', 'tai64c']

sns.set_style("whitegrid")

for inst in instances:
    inst_df = df[df['Instance'] == inst]
    
    if inst_df.empty:
        continue

    plt.figure(figsize=(10, 8))
    
    # 2. Scatter Plot: Cost vs. Similarity to Global Optimum
    # Changed x from 'QualityGap' to 'Cost'
    sns.regplot(
        data=inst_df, x='Cost', y='SimToOpt', 
        label='Similarity to Optimum', scatter_kws={'alpha':0.6}, color='royalblue'
    )
    
    # 3. Scatter Plot: Cost vs. Average Similarity to Others
    # Changed x from 'QualityGap' to 'Cost'
    sns.regplot(
        data=inst_df, x='Cost', y='SimToOther', 
        label='Avg Similarity to Others', scatter_kws={'alpha':0.6}, color='orange', marker='s'
    )

    # 4. Calculate Correlations (r) using raw Cost
    r_opt, _ = pearsonr(inst_df['Cost'], inst_df['SimToOpt'])
    r_other, _ = pearsonr(inst_df['Cost'], inst_df['SimToOther'])
    
    stats_text = (
        f"Instance: {inst}\n"
        f"r (Cost vs SimToOpt): {r_opt:.4f}\n"
        f"r (Cost vs AvgSimilarity): {r_other:.4f}"
    )

    # Annotate plot
    plt.text(0.05, 0.05, stats_text, transform=plt.gca().transAxes, 
             fontsize=12, verticalalignment='bottom', 
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

    # 5. Formatting
    plt.title(f'Fitness-Similarity Analysis: {inst}', fontsize=16)
    plt.xlabel('Solution Cost (Lower is Better)', fontsize=12) # Updated Label
    plt.ylabel('Similarity (Fraction of matching positions)', fontsize=12)
    plt.legend(loc='upper right')
    plt.tight_layout()

    # Save output files
    plt.savefig(f'../results/similarity_{inst}_cost.png', dpi=300)
    plt.close()

print("Similarity Analysis plots (Cost-based) generated successfully.")