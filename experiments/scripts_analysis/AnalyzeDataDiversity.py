import sys
from tabulate import tabulate
import pandas as pd

def get_data_information(df, grouped_df, grouping_columns, runs, algorithm, mutation, constrained):

    df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    df = df.copy()
    df['mean_generations_ratio'] = df['generations'] / df['max_generations']
    if(constrained): 
        df['betterequal_than_opt'] = df['fitness'] <= df['opt']
        df['tardy_share'] = df['fitness'] / df['n']
    merged_df = df.merge(grouped_df, on=grouping_columns, suffixes=('', '_grouped'))
    
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

        result.append(("Ratio of max diversity reached (m=1): ", str(len(merged_df[(merged_df['diversity'] == 1) & (merged_df['m'] == 1)]) / len(merged_df[merged_df['m'] == 1]))))
        result.append(("Ratio of max diversity reached (all m): ", str(len(merged_df[merged_df['diversity'] == 1]) / len(merged_df))))

        result.append(("Average generation ratio (cases with only max diversity): ", str(merged_df[merged_df['diversity'] == 1]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with only max diversity): ", str(merged_df[(merged_df['diversity'] == 1) & (merged_df['diversity_grouped'] == 1)]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity): ", str(merged_df[merged_df['diversity_grouped'] < 1]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity, only cases with max diversity): ", str(merged_df[(merged_df['diversity']) == 1 & (merged_df['diversity_grouped'] < 1)]['mean_generations_ratio'].mean())))

        result.append(("Average generation ratio (cases with only max diversity) m = 1: ", str(merged_df[(merged_df['diversity'] == 1) & (merged_df['m'] == 1)]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with only max diversity) m = 1: ", str(merged_df[(merged_df['diversity'] == 1) & (merged_df['diversity_grouped'] == 1) & (merged_df['m'] == 1) ]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity) m = 1: ", str(merged_df[(merged_df['diversity_grouped'] < 1) & (merged_df['m'] == 1)]['mean_generations_ratio'].mean())))
        result.append(("Average generation ratio (combinations with non-max diversity, only cases with max diversity) m = 1: ", str(merged_df[(merged_df['diversity']) == 1 & (merged_df['diversity_grouped'] < 1) & (merged_df['m'] == 1)]['mean_generations_ratio'].mean())))

        if(constrained):
            for alpha in [0.1]:
                filtered_df = merged_df[merged_df['alpha'] == alpha]
                result.append(("", ""))
                result.append((f"Percentage of cases where OBJ<=OPT, alpha={alpha}: ", str(len(filtered_df[(filtered_df['betterequal_than_opt'] == True)]) / len(filtered_df))))
                result.append((f"Percentage of cases where diversity is 1 and OBJ<=OPT, alpha={alpha}: ", str(len(filtered_df[(filtered_df['diversity'] == 1) & (filtered_df['betterequal_than_opt'] == True)]) / len(filtered_df[filtered_df['diversity'] == 1]))))
                result.append((f"Percentage of cases OBJ<=OPT (m=1), alpha={alpha}: ", str(len(filtered_df[(filtered_df['betterequal_than_opt'] == True) & (filtered_df['m'] == 1)]) / len(filtered_df[(filtered_df['m'] == 1)]))))
                result.append((f"Percentage of cases where diversity is 1 and OBJ<=OPT (m=1), alpha={alpha}: ", str(len(filtered_df[(filtered_df['diversity'] == 1) & (filtered_df['betterequal_than_opt'] == True) & (filtered_df['m'] == 1)]) / len(filtered_df[(filtered_df['diversity'] == 1) & (filtered_df['m'] == 1)]))))
                result.append((f"Average tardy_share, alpha={alpha}: ", str(filtered_df['tardy_share'].mean())))
                result.append((f"Average tardy_share (cases with only max diversity), alpha={alpha}: ", str(filtered_df[filtered_df['diversity'] == 1]['tardy_share'].mean())))
                result.append((f"Average tardy_share, m=1, alpha={alpha}: ", str(filtered_df[(filtered_df['m'] == 1)]['tardy_share'].mean())))
                result.append((f"Average tardy_share (cases with only max diversity), m=1, alpha={alpha}: ", str(filtered_df[(filtered_df['diversity'] == 1) & (filtered_df['m'] == 1)]['tardy_share'].mean())))


    result_str = ""

    max_lengths = [max(len(item) for item in tpl) for tpl in zip(*result)]
    for tpl in result:
        formatted_items = [item.ljust(max_length) for item, max_length in zip(tpl, max_lengths)]
        result_str += ' | '.join(formatted_items) + "\n"

    result_str += "\n" + "Value combinations with not only max diversity:\n" + str(grouped_df[grouped_df['diversity'] != 1][grouping_columns]) + "\n\n"

    if('alpha' in grouping_columns):
        result_str += "Percentage of cases where diversity is 1:, m = 1 \n"
        for alpha in merged_df['alpha'].unique():
            result_str += "alpha: " + str(alpha) + ": " + str(len(merged_df[(merged_df['alpha'] == alpha) & (merged_df['diversity'] == 1) & (merged_df['m'] == 1)]) / len(merged_df[(merged_df['alpha'] == alpha) & (merged_df['m'] == 1)])) + "\n"
    
    if('alpha' in grouping_columns):
        result_str += "Percentage of cases where diversity is 1:, m >= 1 \n"
        for alpha in merged_df['alpha'].unique():
            result_str += "alpha: " + str(alpha) + ": " + str(len(merged_df[(merged_df['alpha'] == alpha) & (merged_df['diversity'] == 1)]) / len(merged_df[(merged_df['alpha'] == alpha)])) + "\n"

    result_str += "\n" + "Value combinations with their occurrences:\n" + str(grouped_df[grouping_columns + ['occurrences']]) + "\n\n"

    return result_str

def max_perc(rows):
    absolute = 0
    for i, row in rows.iterrows():
        if(row['diversity'] == 1): absolute += 1 
    return absolute / len(rows)

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
    summary['max_perc'] = grouped.apply(max_perc).reset_index(drop=True)
    summary['mean_generations_ratio'] = summary['generations'] / summary['max_generations']
    if(constrained): summary['fitness_worse_than_opt'] = summary['fitness'] > summary['opt'] 

    summary = pd.merge(summary, occurrences, on=grouping_columns)   

    return summary

if(__name__ == "__main__"):

    if len(sys.argv) < 6:
        print("Usage: python3 AnalyzeDataDiversity.py <input_file> <output_file_prefix> <grouping_columns> <runs> <constrained> [<algorithms> <mutations>]")
        exit(1)

    input_file = sys.argv[1]
    output_file_prefix = sys.argv[2]
    grouping_columns = sys.argv[3].split(",")
    runs = int(sys.argv[4])
    constrained = sys.argv[5] == "True"
    algorithms = sys.argv[6].split(",") if len(sys.argv) > 6 else None
    mutations = sys.argv[7].split(",") if len(sys.argv) > 7 else None

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