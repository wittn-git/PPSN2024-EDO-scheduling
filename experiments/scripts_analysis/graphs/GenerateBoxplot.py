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
    custom_colors = {'1RAI': "#BA55D3", 'XRAI_0.100000': "#B0E0E6", 'XRAI_0.200000': "#C71585", "XRAI_2.000000": "#513566"} 
    mutation_ordering = {'1RAI':0, 'XRAI_0.100000':1, 'XRAI_0.200000':2, 'XRAI_2.000000':3}
    mutation_labels = ['1(R+I)', 'X(R+I), λ=0.1', 'X(R+I), λ=0.2', 'X(R+I), λ=2']
    for group in unique_groups:
        custom_colors_, mutation_ordering_, mutation_labels_ = custom_colors.copy(), mutation_ordering.copy(), mutation_labels.copy()
        if(group[0] == 1):
            custom_colors_['NSWAP'] = "#7B68EE"
            mutation_ordering_['NSWAP'] = 4
            mutation_labels_.append('N-SWAP')
        filtered_df = df[df[groupings[0]] == group[0]]
        n_values = filtered_df['n'].unique()
        n_values.sort()
        mu_values = [len(filtered_df[(filtered_df['n'] == n)]['mu'].unique()) for n in n_values]
        fig = plt.figure(figsize=(20, 6)) 
        gs = gridspec.GridSpec(1, len(n_values), width_ratios=mu_values)
        axes = [plt.subplot(gs[i]) for i in range(len(n_values))]
        for i, n in enumerate(n_values):
            n_group_data = filtered_df[(filtered_df['n'] == n)]
            sns.boxplot(x='mu', y='generation_ratio', hue='mutation', data=n_group_data, ax=axes[i], hue_order=mutation_ordering_, palette=custom_colors_)
            axes[i].set_xlabel('µ')
            if i == 0: 
                axes[i].set_ylabel('Ratio of generations to max generations')
            else: axes[i].set_ylabel('')
            axes[i].set_title(f'n={n}')
            axes[i].set_ylim(0, 1)
        handles, labels = axes[0].get_legend_handles_labels()
        fig.legend(handles, labels, loc='upper center', title='Operator', ncol=5 if group[0] == 1 else 4, bbox_to_anchor=(0.5, 1), bbox_transform=plt.gcf().transFigure) 
        for i in range(len(n_values)): axes[i].get_legend().remove()
        plt.tight_layout(rect=[0, 0, 1, 0.85])
        plt.savefig(f'{outputfile}_{group}.png')
        plt.close()

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 GenerateBoxplot.py <output_file> <constrained> <div>")
        exit(1)
    
    output_file = sys.argv[1]
    constrained = sys.argv[2] == 'True'
    div = sys.argv[3]
    input_file = f"../../data/diversity_experiments/data_mu1-{'' if constrained else 'un'}const.csv"
    make_plot(input_file, output_file, constrained, div)