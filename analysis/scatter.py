import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import pearsonr
import os

os.makedirs("plots", exist_ok=True)

def plot_scatter_with_correlation(df, instance_name, algo):
    subset = df[(df['Instance'] == instance_name) & (df['Algo'] == algo)]
    if subset.empty:
        return
    
    x = subset['InitCost']
    y = subset['FinalCost']
    
    # Calculate Pearson r and p-value
    r, p_val = pearsonr(x, y)
    
    plt.figure(figsize=(8, 6))
    sns.scatterplot(x=x, y=y, alpha=0.6, edgecolor=None)
    
    # Plot a simple trendline
    sns.regplot(x=x, y=y, scatter=False, color='red', line_kws={'linestyle': '--'})
    
    plt.title(f"Init vs Final Cost | {instance_name} - Algo {algo}")
    plt.xlabel("Initial Solution Cost")
    plt.ylabel("Final Local Optimum Cost")
    
    # Annotate with Pearson r
    plt.annotate(f'Pearson r = {r:.3f}\n(p = {p_val:.3e})', 
                 xy=(0.05, 0.95), xycoords='axes fraction', 
                 fontsize=12, backgroundcolor='white', 
                 verticalalignment='top')
    
    plt.tight_layout()
    plt.savefig(f"plots/scatter_{instance_name}_{algo}.png")
    plt.close()

def main():
    # Assuming your C++ scatter experiment writes to a file like this
    df = pd.read_csv("../results/scatter_results.csv")
    sns.set_theme(style="whitegrid")
    
    # Get unique instances tested in the scatter phase
    instances = df['Instance'].unique()
    
    for inst in instances:
        plot_scatter_with_correlation(df, inst, 'G')
        plot_scatter_with_correlation(df, inst, 'S')
        
    print("Successfully generated scatter plots with Pearson r.")

if __name__ == "__main__":
    main()