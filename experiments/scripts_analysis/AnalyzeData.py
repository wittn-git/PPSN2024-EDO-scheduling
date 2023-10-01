import sys
from tabulate import tabulate
import pandas as pd

def get_data_information(df, grouped_df, grouping_columns, runs, algorithm, mutation, constrained):

    df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    df = df.copy()
    df['mean_generations_ratio'] = df['generations'] / df['max_generations']
    df['fitness_worse_than_opt'] = df['fitness'] < df['opt']
    merged_df = df.merge(grouped_df, on=grouping_columns, suffixes=('', '_grouped'))
    merged_df['diversity'] = merged_df['diversity'] * 100
    
    result = []
    result.append(("Number of datapoints: ", str(len(merged_df))))
    result.append(("Number of duplicate datapoints: ", str(len(merged_df) - len(merged_df.drop_duplicates()))))
    result.append(("Number of value combinations: ", str(len(grouped_df))))

    result.append((f"(After here: removed groups with unequal to {runs} occurrences and without duplicates)", ""))
    merged_df.drop_duplicates()
    merged_df = merged_df[merged_df['occurrences'] == runs]
    if(len(merged_df) != 0):
        result.append(("Number of datapoints: ", str(len(merged_df))))
        result.append(("Number of value combinations: ", str(len(grouped_df[grouped_df['occurrences'] == runs]))))    

        result.append(("Ratio of max diversity reached: ", str(len(merged_df[merged_df['diversity'] == 100]) / len(merged_df))))

        result.append(("Average generation ratio (cases with only max diversity): ", str(merged_df[merged_df['diversity'] == 100]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with only max diversity): ", str(merged_df[(merged_df['diversity'] == 100) & (merged_df['diversity_grouped'] == 100)]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity): ", str(merged_df[merged_df['diversity_grouped'] < 100]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity, only cases with max diversity): ", str(merged_df[(merged_df['diversity']) == 100 & (merged_df['diversity_grouped'] < 100)]['mean_generations_ratio'].mean())))

        result.append(("Percentage of cases where fitness is not worse than opt: ", str(len(merged_df[(merged_df['fitness_worse_than_opt'] == 0)]) / len(merged_df))))
        result.append(("Percentage of cases where diversity is 1 and fitness is not worse than opt: ", str(len(merged_df[(merged_df['diversity'] == 100) & (merged_df['fitness_worse_than_opt'] == 0)]) / len(merged_df))))

    result_str = ""

    max_lengths = [max(len(item) for item in tpl) for tpl in zip(*result)]
    for tpl in result:
        formatted_items = [item.ljust(max_length) for item, max_length in zip(tpl, max_lengths)]
        result_str += ' | '.join(formatted_items) + "\n"

    result_str += "\n" + "Value combinations with not only max diversity:\n" + str(grouped_df[grouped_df['diversity'] != 100][grouping_columns]) + "\n\n"

    if('alpha' in grouping_columns):
        result_str += "Percentage of cases where diversity is 1:\n"
        for alpha in merged_df['alpha'].unique():
            result_str += "alpha: " + str(alpha) + ": " + str(len(merged_df[(merged_df['alpha'] == alpha) & (merged_df['diversity'] == 100)]) / len(merged_df[(merged_df['alpha'] == alpha)])) + "\n"

    result_str += "\n" + "Value combinations with their occurrences:\n" + str(grouped_df[grouping_columns + ['occurrences']]) + "\n\n"

    return result_str

def get_summary(df, grouping_columns, algorithm, mutation, runs, constrained):
    df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    
    grouped = df.groupby(grouping_columns)
    occurrences = grouped.size().reset_index(name='occurrences')
    aggregation_dict = {
        'generations': 'mean',
        'opt': 'mean',
        'diversity': 'mean',
        'max_generations': 'mean'
    }
    if(constrained): aggregation_dict['fitness'] = 'mean'
    summary = grouped.agg(aggregation_dict).reset_index()

    summary['std_generations'] = grouped['generations'].std().reset_index(drop=True)
    summary['diversity'] = summary['diversity'] * 100
    if(constrained): summary['fitness_worse_than_opt'] = grouped.apply(lambda x: (x['fitness'] < x['opt']).sum() / len(x)).reset_index(drop=True)
    summary['diversity_not_1'] = grouped.apply(lambda x: (x['diversity'] != 1).sum() / runs).reset_index(drop=True)
    summary['mean_generations_ratio'] = summary['generations'] / summary['max_generations']
    if(constrained): summary['fitness_worse_than_opt'] = summary['fitness'] < summary['opt'] 

    summary = pd.merge(summary, occurrences, on=grouping_columns)   

    return summary

if(__name__ == "__main__"):

    if len(sys.argv) < 6:
        print("Usage: python3 AnalyzeData.py <input_file> <output_file_prefix> <grouping_columns> <runs> <constrained> [<algorithms> <mutations>]")
        exit(1)

    input_file = sys.argv[1]
    output_file_prefix = sys.argv[2]
    grouping_columns = sys.argv[3].split(",")
    runs = int(sys.argv[4])
    constrained = sys.argv[5] == "True"
    algorithms = sys.argv[6].split(",") if len(sys.argv) > 5 else None
    mutations = sys.argv[7].split(",") if len(sys.argv) > 6 else None

    pd.set_option('display.max_rows', None)
    df = pd.read_csv(input_file)

    algorithm_combinations = df[['algorithm', 'mutation']].drop_duplicates()
    if(algorithms != None): 
        algorithm_combinations = algorithm_combinations[algorithm_combinations['algorithm'].isin(algorithms)]
    if(mutations != None):
        algorithm_combinations = algorithm_combinations[algorithm_combinations['mutation'].isin(mutations)]

    for _, entry in algorithm_combinations.iterrows():
        output_file = output_file_prefix + "_" + entry['algorithm'] + "_" + entry['mutation']
        summary = get_summary(df, grouping_columns, entry['algorithm'], entry['mutation'], runs, constrained)
        summary.to_csv(output_file + "_summary.csv")
        with open(output_file + "_summary.txt", 'w') as f:
            f.write(tabulate(summary, headers='keys', tablefmt='psql'))
        
        with open(output_file + "_data_information.txt", 'w') as f:
            f.write(get_data_information(df, summary, grouping_columns, runs, entry['algorithm'], entry['mutation'], constrained))