import sys
import pandas as pd
import matplotlib.pyplot as plt

def make_plot(outputfile):
    df = pd.read_csv('../../data/component_times.csv')
    grouped = df.groupby(['mu', 'n', 'm', 'component'])['time'].mean().reset_index()
    unique_groups = grouped[['mu', 'n', 'm']].drop_duplicates().values
    grouped['time'] = grouped['time'] / 1000

    cmap = plt.get_cmap("tab10")
    custom_colors = {'evaluate': cmap.colors[1], 'selectSurvivors': cmap.colors[3], 'selectParents': cmap.colors[4], 'mutate': cmap.colors[2]}

    ax = grouped.pivot(index=['mu', 'n', 'm'], columns="component", values='time').plot(kind='bar', color=[custom_colors.get(c, 'gray') for c in grouped['component']])
    
    x_labels = ['(µ={}, n={}, m={})'.format(mu, n, m) for mu, n, m in unique_groups]
    ax.set_xticks(range(len(x_labels)))
    ax.set_xticklabels(x_labels)

    ax.set_ylabel('Computation Time (µs)')
    ax.set_xlabel('Parameter Combinations')

    plt.xticks(rotation=0)
    plt.legend(title='Component', loc='upper left')
    plt.tight_layout()
    plt.savefig(outputfile)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 GenerateComponenttimesGraph.py <output_file>")
        exit(1)
    
    output_file = sys.argv[1]
    make_plot(output_file)
