import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

os.makedirs("plots", exist_ok=True)

def main():
    # Load similarity data. 
    # Assumed columns: Instance, QualityGap, SimToOpt, SimToOther
    df = pd.read_csv("../results/similarity_results.csv")
    sns.set_theme(style="whitegrid")
    
    instances = df['Instance'].unique()
    
    for inst in instances:
        subset = df[df['Instance'] == inst]
        
        plt.figure(figsize=(10, 6))
        
        # Plot Similarity to Optimum
        sns.scatterplot(data=subset, x='QualityGap', y='SimToOpt', 
                        label='Similarity to Optimum', alpha=0.7, marker='o')
        
        # Plot Average Similarity to other local optima
        sns.scatterplot(data=subset, x='QualityGap', y='SimToOther', 
                        label='Avg Similarity to Others', alpha=0.7, marker='x')
        
        plt.title(f"Fitness Landscape Analysis | {inst}")
        plt.xlabel("Quality Gap (% above optimum) -> Closer to 0 is better")
        plt.ylabel("Similarity Metric (e.g., matching edges/positions)")
        
        # Invert X axis so better solutions (0 gap) are on the right, 
        # which makes landscape funnels visually intuitive.
        plt.gca().invert_xaxis() 
        
        plt.legend()
        plt.tight_layout()
        plt.savefig(f"plots/similarity_{inst}.png")
        plt.close()
        
    print("Successfully generated similarity landscape plots.")

if __name__ == "__main__":
    main()