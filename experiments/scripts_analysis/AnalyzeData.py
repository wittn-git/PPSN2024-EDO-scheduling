import pandas as pd
from tabulate import tabulate

constrained = True
runs = 30

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
summary['fitness_worse_than_opt'] = grouped.apply(lambda x: (x['fitness'] < x['opt']).sum() / len(x)).reset_index(drop=True)
std_generations = grouped['generations'].std().reset_index(name='std_generations')

result = pd.merge(summary, occurrences, on=parameters_combinations)
result = pd.merge(result, std_generations, on=parameters_combinations)

result['diversity_not_1'] = grouped.apply(lambda x: (x['diversity'] != 1).sum()).reset_index(drop=True)
result['mean_generations_ratio'] = result['generations'] / result['max_generations']
result.to_csv(res_file+".csv")
with open(res_file+".txt", 'w') as f:
    f.write(tabulate(result, headers='keys', tablefmt='psql'))

# print all combinations with not only max diversity
print(result[result['diversity_not_1'] != 0][parameters_combinations])

result = result[result['occurrences'] == runs]
average_mean_generations_ratio = result[result['diversity_not_1'] == 0]['mean_generations_ratio'].mean()
overall_max_diversity_reached = len(df[df['diversity'] == 1])
total_instances = len(df)
print("Average mean generations ratio (combinations with only max diversity):", average_mean_generations_ratio)
print("Ratio max diversity reached:", overall_max_diversity_reached/total_instances)

filtered_df = df.groupby(parameters_combinations).filter(lambda group: not all(group['diversity'] == 1))
print("Average generations ratio (combinations where there are non-max diversities):", filtered_df['generations'].mean() / filtered_df['max_generations'].mean())
n_not_max_diversity = len(filtered_df)
filtered_df = filtered_df[filtered_df['diversity'] == 1]
n_not_max_diversity_and_max_diversity = len(filtered_df)
print("Ratio of cases where the combination is not max diversity, but the case is:", n_not_max_diversity_and_max_diversity/n_not_max_diversity)
print("Average generations ratio (combinations where there are non-max diversities, but take only the max diversity cases):", filtered_df['generations'].mean() / filtered_df['max_generations'].mean())

# add to df boolean: fitness worse than opt
df['fitness_worse_than_opt'] = df['fitness'] < df['opt'] 
# print percentage of cases where diversity is 1 and fitness is not worse than opt
print("Percentage of cases where diversity is 1 and fitness is not worse than opt:", len(df[(df['diversity'] == 1) & (df['fitness_worse_than_opt'] == 0)]) / len(df))
print("Percentage of cases where fitness is not worse than opt:", len(df[(df['fitness_worse_than_opt'] == 0)]) / len(df))
# print percentage for all the cases where alpha = 0.2 and diversity is 1
print("Percentage of cases where alpha = 0.2 and diversity is 1:", len(df[(df['alpha'] == 0.2) & (df['diversity'] == 1)]) / len(df[(df['alpha'] == 0.2)]))
print("Percentage of cases where alpha = 0.5 and diversity is 1:", len(df[(df['alpha'] == 0.5) & (df['diversity'] == 1)]) / len(df[(df['alpha'] == 0.5)]))
print("Percentage of cases where alpha = 1 and diversity is 1:", len(df[(df['alpha'] == 1) & (df['diversity'] == 1)]) / len(df[(df['alpha'] == 1)]))