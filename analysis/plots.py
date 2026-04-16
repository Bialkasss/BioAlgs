import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Create output directory for plots
os.makedirs("plots", exist_ok=True)

def main():
    # Load data
    df = pd.read_csv("../results/results.csv")
    
    # Calculate derived metrics
    # Gap % = (Final - Opt) / Opt * 100
    df['QualityGap'] = 100.0 * (df['FinalCost'] - df['OptCost']) / df['OptCost']
    
    # Efficiency = Quality Improvement / Time
    # Justification for report: Measures how much cost reduction is achieved per millisecond of compute.
    df['Improvement'] = df['InitCost'] - df['FinalCost']
    df['Efficiency'] = df['Improvement'] / df['TimeMS']

    # Set visual style
    sns.set_theme(style="whitegrid")

    # 1. Quality (% above optimum) Box Plot
    plt.figure(figsize=(10, 6))
    sns.boxplot(data=df, x='Instance', y='QualityGap', hue='Algo')
    plt.title("Solution Quality Gap (% above Optimum)")
    plt.ylabel("Gap (%)")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("plots/quality_gap_boxplot.png")
    plt.close()

    # 2. Runtime comparison (Average + Std Dev)
    plt.figure(figsize=(10, 6))
    sns.barplot(data=df, x='Instance', y='TimeMS', hue='Algo', capsize=.1, errorbar='sd')
    plt.title("Runtime Comparison (Avg ± Std Dev)")
    plt.ylabel("Time (ms)")
    plt.yscale("log") # Log scale is often better if instances vary wildly in size
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("plots/runtime_comparison.png")
    plt.close()

    # 3. Efficiency Box Plot
    plt.figure(figsize=(10, 6))
    sns.boxplot(data=df, x='Instance', y='Efficiency', hue='Algo')
    plt.title("Search Efficiency (Cost Improvement / ms)")
    plt.ylabel("Efficiency")
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig("plots/efficiency_boxplot.png")
    plt.close()

    # 4. Steps and Evals for G and S
    # Filter for only Greedy and Steepest
    df_gs = df[df['Algo'].isin(['G', 'S'])]
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    sns.boxplot(data=df_gs, x='Instance', y='Steps', hue='Algo', ax=axes[0])
    axes[0].set_title("Improving Moves (Steps)")
    axes[0].tick_params(axis='x', rotation=45)
    
    sns.boxplot(data=df_gs, x='Instance', y='Evals', hue='Algo', ax=axes[1])
    axes[1].set_title("Total Delta Evaluations")
    axes[1].tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.savefig("plots/steps_and_evals.png")
    plt.close()

    print("Successfully generated plots.py outputs in the /plots directory.")

if __name__ == "__main__":
    main()