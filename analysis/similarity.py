import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.stats import pearsonr

# 1. Load the similarity experiment data
# Columns: Instance, QualityGap, SimToOpt, SimToOther
df = pd.read_csv('../results/similarity_results.csv')

# Instances to analyze
instances = ['sko81', 'tai64c']

sns.set_style("whitegrid")

for inst in instances:
    inst_df = df[df['Instance'] == inst]
    
    if inst_df.empty:
        continue

    plt.figure(figsize=(10, 8))
    
    # 2. Scatter Plot: Quality Gap vs. Similarity to Global Optimum
    sns.regplot(
        data=inst_df, x='QualityGap', y='SimToOpt', 
        label='Similarity to Optimum', scatter_kws={'alpha':0.6}, color='royalblue'
    )
    
    # 3. Scatter Plot: Quality Gap vs. Average Similarity to Others
    sns.regplot(
        data=inst_df, x='QualityGap', y='SimToOther', 
        label='Avg Similarity to Others', scatter_kws={'alpha':0.6}, color='orange', marker='s'
    )

    # 4. Calculate Correlations (r)
    r_opt, _ = pearsonr(inst_df['QualityGap'], inst_df['SimToOpt'])
    r_other, _ = pearsonr(inst_df['QualityGap'], inst_df['SimToOther'])
    
    stats_text = (
        f"Instance: {inst}\n"
        f"r (Quality vs SimToOpt): {r_opt:.4f}\n"
        f"r (Quality vs AvgSimilarity): {r_other:.4f}"
    )

    # Annotate plot
    plt.text(0.05, 0.05, stats_text, transform=plt.gca().transAxes, 
             fontsize=12, verticalalignment='bottom', 
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

    # 5. Formatting
    plt.title(f'Fitness-Similarity Analysis: {inst}', fontsize=16)
    plt.xlabel('Quality Gap (%) - Lower is Better', fontsize=12)
    plt.ylabel('Similarity (Fraction of matching positions)', fontsize=12)
    plt.legend(loc='upper right')
    plt.tight_layout()

    # Save output files
    plt.savefig(f'../results/similarity_{inst}.png', dpi=300)
    plt.close()

print("Similarity Analysis plots generated successfully.")