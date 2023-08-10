import pandas as pd

constrained = True

csv_file, parameters_combinations = "", []
if constrained:
    csv_file = "../data/test_mu1_constrained.csv"
    parameters_combinations = ['mu', 'n', 'm', 'alpha']
else:
    csv_file = "../data/test_mu1_unconstrained.csv"
    parameters_combinations = ['mu', 'n', 'm']

df = pd.read_csv(csv_file)

num_datapoints = len(df)
num_duplicates = num_datapoints - len(df.drop_duplicates())
value_combinations = df.groupby(parameters_combinations).size().reset_index(name='occurrences')
pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)

print("Number of datapoints:", num_datapoints)
print("Number of duplicate datapoints:", num_duplicates)
print("Number of value combinations:", len(value_combinations))
print("Possible value combinations:\n", value_combinations)