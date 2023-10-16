import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def max_perc(rows):
    absolute = 0
    for i, row in rows.iterrows():
        if(row['diversity'] == 1): absolute += 1 
    return absolute / len(rows)

def make_plot(input_file, output_file, alpha):

    df = pd.read_csv(input_file)
    df = df[df['alpha'] == alpha]

    df_cp = df.copy()
    group_cols = ['mu', 'n', 'm', 'mutation']

    df_cp.loc[:, 'late_ratio'] = df_cp['fitness'] / df_cp['n']
    
    grouped = df_cp.groupby(group_cols)
    summary = grouped.agg({'diversity': "mean", 'late_ratio': "mean", 'opt': "mean"}).reset_index()
    summary['max_perc'] = grouped.apply(max_perc).reset_index(drop=True)
    occurrences = grouped.size().reset_index(name='occurrences')
    summary = pd.merge(summary, occurrences, on=group_cols)
    

    fig, ax = plt.subplots(1, 2, figsize=(10, 5))
    custom_colors = {'1RAI': "#BA55D3", 'XRAI_0.100000': "#B0E0E6", 'XRAI_0.200000': "#C71585", "XRAI_2.000000": "#513566", "NSWAP": "#7B68EE"} 

    noise_level = 0.003
    for mutation in summary['mutation'].unique():
        temp_df = summary[(summary['mutation'] == mutation)]
        x_noise = np.random.normal(0, noise_level, len(temp_df['diversity']))
        y_noise = np.random.normal(0, noise_level, len(temp_df['diversity']))

        noisy_div = temp_df['diversity'] + x_noise
        noisy_ratio = temp_df['late_ratio'] + y_noise
        noisy_perc = temp_df['max_perc'] + y_noise
        noisy_div[noisy_div > 1] = 1
        ax[0].scatter(noisy_div, noisy_ratio, s=3, c=custom_colors[mutation])
        ax[1].scatter(noisy_div, noisy_perc, s=3, c=custom_colors[mutation])

    ax[0].set_xlabel("Diversity")
    ax[0].set_ylabel("Fitness")
    ax[0].set_title("Diversity vs Fitness")
    ax[1].set_xlabel("Diversity")
    ax[1].set_ylabel("Max %")
    ax[1].set_title("Diversity vs Max %")
    fig.savefig(output_file + ".png")
    plt.close(fig)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 GenerateBoxplot.py <output_file> <alpha>")
        exit(1)
    
    output_file = sys.argv[1]
    alpha = float(sys.argv[2])

    input_file = f"../../data/diversity_experiments/data_mu1-const.csv"
    make_plot(input_file, output_file, alpha)