import pandas as pd
from tabulate import tabulate

constrained = True

csv_file, parameters_combinations, res_file = "", [], ""
if constrained:
    csv_file = "../data/test_mu1_constrained.csv"
    parameters_combinations = ['mu', 'n', 'm', 'alpha']
    res_file = "res_constrained"
else:
    csv_file = "../data/test_mu1_unconstrained.csv"
    parameters_combinations = ['mu', 'n', 'm']
    res_file = "res_unconstrained"

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)

df = pd.read_csv(csv_file)

grouped = df.groupby(parameters_combinations)
occurrences = grouped.size().reset_index(name='occurrences')
summary = grouped.agg({
    'generations': 'mean',
    'fitness': 'mean',
    'opt': 'mean',
    'diversity': 'mean',
    'max_generations': 'mean'
}).reset_index()

summary['diversity'] = summary['diversity'] * 100
summary['fitness_worse_than_opt'] = grouped.apply(lambda x: (x['fitness'] < x['opt']).sum()).reset_index(drop=True)
std_generations = grouped['generations'].std().reset_index(name='std_generations')

result = pd.merge(occurrences, summary, on=parameters_combinations)
result = pd.merge(result, std_generations, on=parameters_combinations)

# TODO number of times div was maximally high
# TODO interpolate runtime

result.to_csv(res_file+".csv")
with open(res_file+".txt", 'w') as f:
    f.write(tabulate(result, headers='keys', tablefmt='psql'))