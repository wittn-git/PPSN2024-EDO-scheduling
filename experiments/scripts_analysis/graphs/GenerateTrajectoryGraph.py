import sys
import pandas as pd
import matplotlib.pyplot as plt

def make_plot(input_file, output_file, constrained, filters):
    df = pd.read_csv(input_file)
    grouped = df.groupby(['mu', 'n', 'run'])
    for (mu, n, run), group in grouped:
        if((mu, n, run) not in filters): continue
        
        fig, ax = plt.subplots()

        if(constrained):
            alpha_groups = group.groupby(['alpha', 'm', 'mutation'])
            for (alpha, m, operator), alpha_group in alpha_groups:
                if(alpha not in filters[(mu, n, run)]['alpha'] or m not in filters[(mu, n, run)]['m'] or operator not in filters[(mu, n, run)]['operator']):
                    continue
                ax.plot(alpha_group['generations'], alpha_group['diversity'], label=f'Alpha {alpha}, m {m}, operator {operator}')
        else:
            m_groups = group.groupby(['m', 'mutation'])
            for (m, operator), m_group in m_groups:
                if(m not in filters[(mu, n, run)]['m'] or operator not in filters[(mu, n, run)]['operator']):
                    continue
                ax.plot(m_group['generations'], m_group['diversity'], label=f'm {m}, operator {operator}')       

        ax.set_xlabel('Generation')
        ax.set_ylabel('Diversity')
        ax.set_title(f'µ={mu}, n={n}')
        ax.legend()

        plt.savefig(f'{output_file}_mu_{mu}_n_{n}_run_{run}_plot.png')
        plt.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 GenerateTrajectoryGraph.py <output_file> <constrained>")
        exit(1)
    
    output_file = sys.argv[1]
    constrained = sys.argv[2] == 'True'
    input_file = f"../../data/trajectory_experiments/output_mu1-{'' if constrained else 'un'}const_trajectory.csv"

    filters_const = {
        (10,5,8): {
            "alpha": [0.1, 0.3, 0.6],
            "operator": ["XRAI_0.100000"],
            "m": [3],
        }
    }

    filters_unconst = {

    }

    filters = filters_const if constrained else filters_unconst
    make_plot(input_file, output_file, constrained, filters)
