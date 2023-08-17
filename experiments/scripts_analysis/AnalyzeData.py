import sys
from tabulate import tabulate
import pandas as pd

def get_data_information(df, grouped_df, grouping_columns):

    df['diversity'] = df['diversity'] * 100
    merged_df = df.merge(grouped_df, on=grouping_columns, suffixes=('', '_grouped'))
    df['mean_generations_ratio'] = df['generations'] / df['max_generations']
    df['fitness_worse_than_opt'] = df['fitness'] < df['opt']
    

    result = []
    result.append(("Number of datapoints: ", str(len(df))))
    result.append(("Number of duplicate datapoints: ", str(len(df) - len(df.drop_duplicates()))))
    result.append(("Number of value combinations: ", str(len(grouped_df))))

    result.append(("Ratio of max diversity reached: ", str(len(df[df['diversity'] == 100]) / len(df))))

    result.append(("Average generation ratio (cases with only max diversity): ", str(df[df['diversity'] == 100]['mean_generations_ratio'].mean())))
    result.append(("Average generation ratio (combinations with only max diversity): ", str(merged_df[(merged_df['diversity'] == 100) & (merged_df['diversity_grouped'] == 100)]['mean_generations_ratio'].mean())))
    result.append(("Average generation ratio (combinations with non-max diversity): ", str(merged_df[merged_df['diversity_grouped'] < 100]['mean_generations_ratio'].mean())))
    result.append(("Average generation ratio (combinations with non-max diversity, only cases with max diversity): ", str(merged_df[(merged_df['diversity']) == 100 & (merged_df['diversity_grouped'] < 100)]['mean_generations_ratio'].mean())))

    result.append(("Percentage of cases where fitness is not worse than opt: ", str(len(df[(df['fitness_worse_than_opt'] == 0)]) / len(df))))
    result.append(("Percentage of cases where diversity is 1 and fitness is not worse than opt: ", str(len(df[(df['diversity'] == 100) & (df['fitness_worse_than_opt'] == 0)]) / len(df))))

    result_str = ""

    max_lengths = [max(len(item) for item in tpl) for tpl in zip(*result)]
    for tpl in result:
        formatted_items = [item.ljust(max_length) for item, max_length in zip(tpl, max_lengths)]
        result_str += ' | '.join(formatted_items) + "\n"

    result_str += "\n" + "Value combinations with not only max diversity:\n" + str(grouped_df[grouped_df['diversity'] != 100][grouping_columns]) + "\n\n"

    if('alpha' in grouping_columns):
        result_str += "Percentage of cases where diversity is 1:\n"
        for alpha in df['alpha'].unique():
            result_str += "alpha: " + str(alpha) + ": " + str(len(df[(df['alpha'] == alpha) & (df['diversity'] == 100)]) / len(df[(df['alpha'] == alpha)])) + "\n"

    return result_str

def get_summary(df, grouping_columns, runs):
    grouped = df.groupby(grouping_columns)
    occurrences = grouped.size().reset_index(name='occurrences')
    summary = grouped.agg({
        'generations': 'mean',
        'fitness': 'mean',
        'opt': 'mean',
        'diversity': 'mean',
        'max_generations': 'mean'
    }).reset_index()

    summary['std_generations'] = grouped['generations'].std().reset_index(drop=True)
    summary['diversity'] = summary['diversity'] * 100
    summary['fitness_worse_than_opt'] = grouped.apply(lambda x: (x['fitness'] < x['opt']).sum() / len(x)).reset_index(drop=True)
    summary['diversity_not_1'] = grouped.apply(lambda x: (x['diversity'] != 1).sum()).reset_index(drop=True)
    summary['mean_generations_ratio'] = summary['generations'] / summary['max_generations']
    summary['fitness_worse_than_opt'] = summary['fitness'] < summary['opt'] 

    summary = pd.merge(summary, occurrences, on=grouping_columns)   

    return summary

if(__name__ == "__main__"):

    if len(sys.argv) < 5:
        print("Usage: python3 AnalyzeData.py <input_file> <output_file_name> <grouping_columns> <runs>")
        exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    grouping_columns = sys.argv[3].split(",")
    runs = int(sys.argv[4])

    pd.set_option('display.max_rows', None)
    df = pd.read_csv(input_file)

    summary = get_summary(df, grouping_columns, runs)
    summary.to_csv(output_file + "_summary.csv")
    with open(output_file + "_summary.txt", 'w') as f:
        f.write(tabulate(summary, headers='keys', tablefmt='psql'))
    
    with open(output_file + "_data_information.txt", 'w') as f:
        f.write(get_data_information(df, summary, grouping_columns))