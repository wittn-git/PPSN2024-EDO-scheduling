import sys
import pandas as pd
import matplotlib.pyplot as plt
import itertools

def make_plot(input_file, output_file, constrained, combinations):
    df = pd.read_csv(input_file)
    
    grouped = df.groupby(['mu', 'n', 'run'])
    mutation_labels = {'1RAI': '1(R+I)', 'XRAI_0.100000': 'X(R+I), λ = 0.1', 'XRAI_0.200000': 'X(R+I), λ = 0.2', "XRAI_2.000000": 'X(R+I), λ = 2', 'NSWAP': 'N-SWAP'}

    plt.rcParams.update({'font.size': 14})
    fig, axes = plt.subplots(1, len(combinations), figsize=(16, 5))
    if len(combinations) == 1: axes = [axes]
    for i, entry in enumerate(combinations):
        custom_colors = ["#BA55D3","#B0E0E6","#C71585", "#513566", "#7B68EE"]
        color_cycle = itertools.cycle(custom_colors)
        for (mu, n, run), group in grouped:
            if(mu not in entry['mu'] or n not in entry['n'] or run not in entry['run']): continue

            if(constrained):
                alpha_groups = group.groupby(['alpha', 'm', 'mutation'])
                for (alpha, m, operator), alpha_group in alpha_groups:
                    if(alpha not in entry['alpha'] or m not in entry['m'] or operator not in entry['operator']):
                        continue
                    color_ = next(color_cycle)
                    label_items = []
                    if len(entry["operator"]) > 1: label_items.append(f'{mutation_labels[operator]}')
                    if len(entry["m"]) > 1: label_items.append(f'm = {m}')
                    if len(entry["alpha"]) > 1: label_items.append(f'α = {alpha}')
                    axes[i].plot(alpha_group['generations'], alpha_group['diversity'], label=", ".join(label_items), color=color_)
            else:
                m_groups = group.groupby(['m', 'mutation'])
                for (m, operator), m_group in m_groups:
                    if(m not in entry['m'] or operator not in entry['operator']):
                        continue
                    color_ = next(color_cycle)
                    label_items = []
                    if len(entry["operator"]) > 1: label_items.append(f'{mutation_labels[operator]}')
                    if len(entry["m"]) > 1: label_items.append(f'm = {m}')
                    axes[i].plot(m_group['generations'], m_group['diversity'], label=", ".join(label_items), color=color_)      

            if i == 0: 
                axes[i].set_ylabel('Diversity')
            else: axes[i].set_ylabel('')
            axes[i].set_xlabel('Generation')
            axes[i].legend()
            transform = {"alpha": "α", "m": "m", "mutation": "mutation", "operator": "operator", "run": "run", "mu": "μ", "n": "n"}
            title_string = ", ".join([f'{transform[k]} = {v[0]}' for k, v in entry.items() if len(v) == 1 and k != 'operator' and k != 'run'])
            if len(entry["operator"]) == 1: title_string += f', {mutation_labels[entry["operator"][0]]}'
            axes[i].set_title(title_string)

    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 GenerateTrajectoryGraph.py <output_file> <constrained>")
        exit(1)
    
    output_file = sys.argv[1]
    constrained = sys.argv[2] == 'True'
    input_file = f"../../data/trajectory_experiments/output_mu1-{'' if constrained else 'un'}const_trajectory.csv"

    combinations_const = [
        {
            "mu": [10],
            "n": [25],
            "run": [1],
            "alpha": [0.1],
            "operator": ['1RAI', 'XRAI_0.100000', 'XRAI_0.200000', "XRAI_2.000000"],
            "m": [10]
        },
        {
            "mu": [10],
            "n": [25],
            "run": [1],
            "alpha": [0.1, 0.3, 0.6],
            "operator": ['1RAI'],
            "m": [1]
        },
        {
            "mu": [10],
            "n": [25],
            "run": [1],
            "alpha": [0.1],
            "operator": ["1RAI"],
            "m": [1,3,5,10]
        },
    ]

    combinations_unconst = [
        {
            "mu": [25],
            "n": [25],
            "run": [2],
            "operator": ['1RAI', 'XRAI_0.100000', 'XRAI_0.200000', "XRAI_2.000000", "NSWAP"],
            "m": [10]
        },
        {
            "mu": [2],
            "n": [25],
            "run": [15],
            "operator": ['1RAI', 'XRAI_0.100000', 'XRAI_0.200000', "XRAI_2.000000", "NSWAP"],
            "m": [1]
        },
        {
            "mu": [10],
            "n": [25],
            "run": [0],
            "operator": ['1RAI'],
            "m": [1,3,5,10]
        }
    ]

    combinations = combinations_const if constrained else combinations_unconst
    make_plot(input_file, output_file, constrained, combinations)
