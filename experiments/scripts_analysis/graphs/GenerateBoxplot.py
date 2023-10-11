import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import gridspec
from icecream import ic
import seaborn as sns

def make_plot(input_file, outputfile, constrained, div):
    df = pd.read_csv(input_file)
    if(div == 'max'):
        df = df[df['diversity'] == 1]
    elif(div == 'all'):
        pass
    else:
        print("Invalid diversity option")
        exit(1)
    df['generation_ratio'] = df['generations'] / df['max_generations']
    groupings = ['m']
    if(constrained):
        groupings.append('alpha')
    unique_groups = df[groupings].drop_duplicates().values
    custom_colors = {'1RAI': "#BA55D3", 'NSWAP': "#7B68EE", 'XRAI_0.100000': "#B0E0E6", 'XRAI_0.200000': "#C71585", "XRAI_2.000000": "#B0E0E6"} # TODO add color to palatte
    mutation_ordering = ['1RAI', 'XRAI_0.100000', 'XRAI_0.200000', 'XRAI_2.000000', 'NSWAP']
    mutation_labels = ['1(R+I)', 'X(R+I), λ=0.1', 'X(R+I), λ=0.2', 'X(R+I), λ=2', 'N-SWAP']
    for group in unique_groups:
        n_values = df[df[groupings[0]] == group[0]]['n'].unique()
        n_values.sort()
        mu_values = [len(df[(df['n'] == n) & (df[groupings[0]] == group[0])]['mu'].unique()) for n in n_values]
        plt.figure(figsize=(20, 5)) 
        gs = gridspec.GridSpec(1, len(n_values), width_ratios=mu_values)
        axes = [plt.subplot(gs[i]) for i in range(len(n_values))]
        for i, n in enumerate(n_values):
            n_group_data = df[(df['n'] == n) & (df[groupings[0]] == group[0])]
            sns.boxplot(x='mu', y='generation_ratio', hue='mutation', data=n_group_data, ax=axes[i], palette=custom_colors, order=mutation_ordering)
            axes[i].set_xlabel('µ')
            if i == 0: axes[i].set_ylabel('generation_ratio')
            else: axes[i].set_ylabel('')
            if i != 2: 
                axes[i].get_legend().remove()
            else:
                handles, _ = axes[i].get_legend_handles_labels()
                axes[i].legend(handles, mutation_labels, title='Operator')
            axes[i].set_title(f'n={n}')
            axes[i].set_ylim(0, 1)
            
        plt.tight_layout()
        plt.savefig(f'{outputfile}_{group}.png')

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 GenerateBoxplot.py <output_file> <constrained> <div>")
        exit(1)
    
    output_file = sys.argv[1]
    constrained = sys.argv[2] == 'True'
    div = sys.argv[3]
    input_file = f"../../data/diversity_experiments/data_mu1-{'' if constrained else 'un'}const.csv"
    make_plot(input_file, output_file, constrained, div)