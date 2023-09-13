import sys
import pandas as pd
import matplotlib.pyplot as plt

def make_plot(outputfile):

    df = pd.read_csv('../../data/data_survivor-opt.csv')
    df['runtime'] = df['runtime'] / df['generations']
    grouped = df.groupby(['mu', 'n', 'm', 'type'])['runtime'].mean().reset_index()
    unique_groups = grouped[['mu', 'n', 'm']].drop_duplicates().values

    custom_colors = {'optimized': "#B0E0E6", 'unoptimized': "#C71585"}
    x_labels = ['(µ={}, n={}, m={})'.format(mu, n, m) for mu, n, m in unique_groups]

    ax = grouped.pivot(index=['mu', 'n', 'm'], columns="type", values='runtime').plot(kind='bar', alpha=0.65, color=[custom_colors.get(c, 'gray') for c in grouped['type']])
    
    ax.set_ylabel('Computation Time (µs) per Generation')
    ax.set_xlabel('Parameter Combinations')
    ax.set_xticks(range(len(x_labels)))
    ax.set_xticklabels(x_labels, rotation=0)
    ax.tick_params(axis='y')

    plt.gca().get_legend().set_title('')
    plt.gcf().set_size_inches(8, 5)

    plt.savefig(outputfile)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 GenerateSurvivorTimesGraph.py <output_file>")
        exit(1)
    
    output_file = sys.argv[1]
    make_plot(output_file)