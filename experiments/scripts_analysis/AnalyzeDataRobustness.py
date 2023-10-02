import sys
from tabulate import tabulate
import pandas as pd

def get_data_information(df, grouped_df, grouping_columns, runs, algorithm, mutation, tests_n):

    df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    df = df.copy()
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
        valid_robustness_tests = 0
        positive_robustness_tests_init, positive_robustness_tests_notinit = 0, 0
        for row in merged_df.iterrows():
            for i in range(tests_n):
                if(row[1][f'rob_test_{i}'] >= -1):
                    valid_robustness_tests += 1
                if(row[1][f'rob_test_{i}'] >= 0):
                    if(row[1]['init'] == 1):
                        positive_robustness_tests_init += 1
                    else:
                        positive_robustness_tests_notinit += 1
        result.append(("Number of valid robustness tests: ", str(valid_robustness_tests)))
        result.append(("Percentage of positive tests (init): ", str(positive_robustness_tests_init/valid_robustness_tests)))
        result.append(("Percentage of positive tests (not init): ", str(positive_robustness_tests_notinit/valid_robustness_tests)))

    result_str = ""

    max_lengths = [max(len(str(item)) for item in tpl) for tpl in zip(*result)]
    for tpl in result:
        formatted_items = [item.ljust(max_length) for item, max_length in zip(tpl, max_lengths)]
        result_str += ' | '.join(formatted_items) + "\n"

    return result_str

def calc_robustness(group, tests_n):
    series = {}
    for i in range(tests_n):
        attribute = f"rob_test_{i}"
        column1, column2 = 'Perc_'+attribute, 'Mean_'+attribute
        if len(group[group[attribute] == -2]) > 0:
            series[column1] = '-'
            series[column2] = '-'
        else:
            total_count = len(group)
            non_negative_count = len(group[group[attribute] >= 0])
            if(non_negative_count == 0):
                average_value = "-"
            else:
                average_value = group[group[attribute] >= 0][attribute].mean()
            series[column1] = non_negative_count / total_count
            series[column2] = average_value
    
    return pd.Series(series)

def get_summary(df, grouping_columns, algorithm, mutation, tests_n):
    df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    
    grouped = df.groupby(grouping_columns)
    occurrences = grouped.size().reset_index(name='occurrences')
    summary = grouped.agg({ 'diversity': 'mean' }).reset_index()
    summary = pd.merge(summary, occurrences, on=grouping_columns)   

    robustness_metrics = grouped.apply(calc_robustness, tests_n=tests_n)
    summary = robustness_metrics.merge(summary, on=grouping_columns)

    return summary

if(__name__ == "__main__"):
    
    if len(sys.argv) < 6:
        print("Usage: python3 AnalyzeData.py <input_file> <output_file_prefix> <grouping_columns> <runs> <tests_n> [<algorithms> <mutations>]")
        exit(1)

    input_file = sys.argv[1]
    output_file_prefix = sys.argv[2]
    grouping_columns = sys.argv[3].split(",") + ['init']
    runs = int(sys.argv[4])
    tests_n = int(sys.argv[5])
    algorithms, mutations = None, None
    if(len(sys.argv) > 6): algorithms = sys.argv[6].split(",") if len(sys.argv) > 5 else None
    if(len(sys.argv) > 7):mutations = sys.argv[7].split(",") if len(sys.argv) > 6 else None
    
    pd.set_option('display.max_rows', None)
    df = pd.read_csv(input_file)

    algorithm_combinations = df[['algorithm', 'mutation']].drop_duplicates()
    if(algorithms != None): 
        algorithm_combinations = algorithm_combinations[algorithm_combinations['algorithm'].isin(algorithms)]
    if(mutations != None):
        algorithm_combinations = algorithm_combinations[algorithm_combinations['mutation'].isin(mutations)]

    for _, entry in algorithm_combinations.iterrows():
        output_file = output_file_prefix + "_" + entry['algorithm'] + "_" + entry['mutation']
        summary = get_summary(df, grouping_columns, entry['algorithm'], entry['mutation'], tests_n)
        summary.to_csv(output_file + "_summary.csv")
        with open(output_file + "_summary.txt", 'w') as f:
            f.write(tabulate(summary, headers='keys', tablefmt='psql'))
        
        with open(output_file + "_data_information.txt", 'w') as f:
            f.write(get_data_information(df, summary, grouping_columns, runs, entry['algorithm'], entry['mutation'], tests_n))