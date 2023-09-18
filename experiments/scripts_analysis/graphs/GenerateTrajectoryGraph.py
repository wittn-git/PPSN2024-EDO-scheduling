import sys
import pandas as pd
import matplotlib.pyplot as plt

def make_plot(input_file, output_file, constrained):
    df = pd.read_csv(input_file)
    grouped = df.groupby(['mu', 'n'])
    for (mu, n), group in grouped:
        fig, ax = plt.subplots()

        if(constrained):
            # Group the data within this subgroup by 'alpha'
            alpha_groups = group.groupby(['alpha', 'm'])
            for (alpha, m), alpha_group in alpha_groups:
                ax.plot(alpha_group['generations'], alpha_group['diversity'], label=f'Alpha {alpha}, m {m}')
        else:
            m_groups = group.groupby(['m'])
            for m, m_group in m_groups:
                ax.plot(m_group['generations'], m_group['diversity'], label=f'm {m}')       

        ax.set_xlabel('Generation')
        ax.set_ylabel('Diversity')
        ax.set_title(f'µ={mu}, n={n}')
        ax.legend()

        plt.savefig(f'{output_file}_mu_{mu}_n_{n}_plot.png')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 GenerateComponenttimesGraph.py <output_file> <constrained>")
        exit(1)
    
    output_file = sys.argv[1]
    constrained = sys.argv[2] == 'True'
    input_file = f"../../data/trajectory_experiments/data_mu1-{'' if constrained else 'un'}const_trajectory_summarized.csv"

    make_plot(input_file, output_file, constrained)