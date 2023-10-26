import pandas as pd
import sys
from icecream import *

def format_float(value):
    try:
        return '{:.2f}'.format(float(value))
    except ValueError:
        return value

def format_doublefloat(value):
    try:
        string = '{:.4f}'.format(float(value))
        if(string == "1.0000" and value < 1):
            return "0.9999"
        if(string == "0.0000" and value > 0):
            return "0.0001"
        return string
    except ValueError:
        return value
    
def format_int(value):
    if isinstance(value, bool):
        return str(value)
    try:
        return '{:.0f}'.format(int(value))
    except ValueError:
        return value

def filter_dataframe(df, columns, filtered):
    filtered_rows = []
    num_cols = len(columns)

    for tup in filtered:
        condition = df[columns[0]] == tup[0]
        for i in range(1, num_cols):
            condition = condition & (df[columns[i]] == tup[i])
        filtered_rows.append(df[condition])

    filtered_df = pd.concat(filtered_rows).drop_duplicates().reset_index()
    return filtered_df

def get_table(csv_file, shown_header, actual_header, grouping_attributes, grouping_header, decimal_columns, double_decimal_columns, upper_header, filtered, highlight_cols, exclude_rows, highlight_max, highlight_colors, description, scriptsize=True):
    
    dataframes = [pd.read_csv(file) for file in csv_file]
    # const size filters
    '''
    for i, df in enumerate(dataframes):
        dataframes[i] = df[df['mu'] <= 10]
        dataframes[i] = dataframes[i][~((dataframes[i]['mu'] == 10) & (dataframes[i]['n'] > 50))]
        dataframes[i] = dataframes[i][~((dataframes[i]['mu'] == 10) & (dataframes[i]['n'] > 25) & (dataframes[i]['m'] > 1))]
    for i, df in enumerate(dataframes):
        dataframes[i] = df[df['mu'] >= 10]
        dataframes[i] = dataframes[i][~((dataframes[i]['mu'] == 10) & (dataframes[i]['n'] < 50))]
        dataframes[i] = dataframes[i][~((dataframes[i]['mu'] == 10) & (dataframes[i]['n'] == 50) & (dataframes[i]['m'] == 1))]
    '''

    for i, df in enumerate(dataframes):
        sorting_ascending = [False if x == 'init' else True for x in grouping_attributes]
        dataframes[i] = df.sort_values(by=grouping_attributes, ascending=sorting_ascending).reset_index().drop(columns=['index'])
        if(filtered != None):
            dataframes[i] = filter_dataframe(dataframes[i], grouping_attributes, filtered)
   
    for column in actual_header:
        for i, df in enumerate(dataframes):
            if(column in decimal_columns):
                dataframes[i][column] = df[column].apply(format_float)
            elif(column in double_decimal_columns):
                dataframes[i][column] = df[column].apply(format_doublefloat)
            else:
                dataframes[i][column] = df[column].apply(format_int)
   
    ungrouped_header = [x for x in shown_header if x not in grouping_header]

    latex_table = "\\begin{center}\n"
    latex_table += "\\renewcommand{\\tabcolsep}{4pt}\n"
    latex_table += " \\renewcommand{\\arraystretch}{1.1}\n"
    if scriptsize: latex_table += " \\begin{scriptsize}\n"
    if("max_perc" in decimal_columns):
        latex_table += "\\begin{tabular}{*{" + str((len(grouping_attributes)+len(ungrouped_header) * len(dataframes))) + "}{>{\\raggedleft\\arraybackslash}p{1cm}}}"
    else: latex_table += f" \\begin{{{'tabular'}}}{{{'r'*(len(grouping_attributes)+len(ungrouped_header) * len(dataframes))}}}\n"
    latex_table += " \\toprule\n"
    latex_table += f"\multicolumn{{{len(grouping_header)}}}{{{'c'}}}{{{''}}}" 
    for header in upper_header:
        latex_table += f" & \multicolumn{{{len(ungrouped_header)}}}{{{'c'}}}{{{header}}}"
    latex_table += "\\\\ \n"
    for i in range(len(dataframes)):
        latex_table += f"\cmidrule(lr){{{str(1+ len(grouping_header) + i*len(ungrouped_header)) + '-' + str(len(grouping_header) + (i+1)*len(ungrouped_header))}}} "
    latex_table += "\n" + "& ".join(grouping_header) + ("&" + " & ".join(ungrouped_header)) * len(dataframes) + "\\\\ \n"

    included_attributes = []
    occurence_dict = {}
    for attribute in grouping_attributes:
        included_attributes.append(attribute)
        grouped = dataframes[0].groupby(included_attributes)
        for row in grouped.size().reset_index().values:
            keys = [str(x) for x in row[:-1]]
            occurence_dict[",".join(keys)] = row[-1]

    currents = {attr: -1 for attr in grouping_attributes}

    highlights = {col: {} for col in highlight_cols}
    for i, row in enumerate(dataframes[0].iterrows()):
        valid_row = True
        for attr, val in exclude_rows.items():
            if(dataframes[0].at[i, attr] == val):
                valid_row = False
                break
        if(not valid_row): continue

        group_values = [dataframes[0].at[i, attr] for attr in grouping_attributes]

        for j, col in enumerate(highlight_cols):
            max_x = None
            max_x_idx = []
            for idx, df in enumerate(dataframes):
                mask = pd.Series(True, index=df.index)
                for attribute, value in zip(grouping_attributes, group_values):
                    mask = mask & (df[attribute] == value)
                if(df[mask].empty): continue
                current_x = float(df[mask][col].values[0])
                if (max_x is None) or (current_x > max_x and highlight_max[j] and current_x != 0) or (current_x < max_x and not highlight_max[j]):
                    if(highlight_max[j] and current_x == 0): continue
                    max_x = current_x
                    max_x_idx = [idx]
                    continue    
                if max_x == current_x:
                    max_x_idx.append(idx)
            if max_x != None:
                highlights[col][i] = max_x_idx
    for i, row in enumerate(dataframes[0].iterrows()):
        first_row = row[1]
        columns = []
        new_group = False
        for attribute in actual_header:
            if(attribute not in currents): continue
            if(currents[attribute] == -1 or currents[attribute] != first_row[attribute] or new_group):
                if(not new_group): 
                    new_group = True
                    if(attribute != grouping_attributes[-1]):
                        latex_table += f"\cline{{{grouping_attributes.index(attribute)+1}-{len(grouping_header) + len(ungrouped_header) * len(dataframes)}}}\n"
                currents[attribute] = first_row[attribute]
                occ_keys = []
                for attr in grouping_attributes:
                    occ_keys.append(str(currents[attr]))
                    if(attr == attribute): break
                columns.append(f"\multirow{{{occurence_dict[','.join(occ_keys)]}}}{{{'*'}}}{{{first_row[attribute]}}}") 
            else: columns.append("")
        for j, df in enumerate(dataframes):
            for attribute in actual_header: 
                if(attribute not in currents):
                    row_ = df
                    for attr, val in currents.items():
                        row_ = row_[row_[attr] == val]
                    highlight_str =  f"\cellcolor{{{highlight_colors[attribute]}}}" if attribute in highlight_cols and i in highlights[attribute] and j in highlights[attribute][i] else ""
                    if(row_.empty): 
                        columns.append(highlight_str + "---")
                    else:
                        columns.append(highlight_str + str(row_[attribute].values[0]))


        latex_table += " & ".join(columns) + "\\\\ \n"
    
    latex_table += " \\bottomrule\n"
    latex_table += f"\multicolumn{{{len(grouping_attributes)+len(ungrouped_header) * len(dataframes)}}}{{{'l'}}}" + "{"
    for i, header in enumerate(ungrouped_header):
        latex_table += header + " " + description[i] + (", " if i < len(ungrouped_header)-1 else "")
    latex_table += "} \\\\ \n"
    latex_table += "  \end{tabular}\n"
    if scriptsize: latex_table += " \end{scriptsize}\n"
    latex_table += " \end{center}\n"
    return latex_table

if __name__ == "__main__":

    if len(sys.argv) < 4:
        print("Usage: python3 CreateTable.py <table_type> <constrained> <outputfile>")
        exit(1)

    table_type = sys.argv[1]
    constrained = sys.argv[2] == "True"
    outputfile = sys.argv[3]
    filtered = None

    upper_header = ["$1(R+I)$", "$X(R+I), \lambda = 0.1$", "$X(R+I), \lambda = 0.2$", "$X(R+I), \lambda = 2$", "$N-SWAP$"]
    grouping = ['mu', 'n', 'm']
    grouping_header = ["$\mu$", "$n$", "$m$"]
    decimal_columns = []
    double_decimal_columns = ['diversity']
    highlight_cols, exclude_rows, highlight_max, highlight_colors = [], {}, [], {}
    description = []
    if(constrained):
        grouping.append('alpha')
        grouping_header.append("$\\alpha$")
        decimal_columns.append('alpha')
    if(table_type == "robustness"):
        grouping.append('init')
        grouping_header.append("$init$")
    
    if(table_type == "fitness"):
        header = grouping_header + ["$D_0$", "$\Delta O$", "$O\%$"]
        columns = grouping + ['diversity', 'opt_diff', 'opt_perc']
        decimal_columns += ['opt_diff', 'opt_perc']
        highlight_cols = ['opt_diff', 'opt_perc']
        highlight_max = [False, True]
        highlight_colors = {'opt_diff': 'lightgray', 'opt_perc': 'gray'}
        description = ['diversity', 'average difference of objective to optimum tardy jobs to total jobs ratio', 'share of cases where objective value is optimal']
        '''filtered = [
            (2, 5, 1, 0.1),
            (2, 5, 1, 0.3),
            (2, 5, 1, 0.6),
            (2, 5, 3, 0.1),
            (10, 10, 1, 0.1),
            (10, 10, 3, 0.1),
            (10, 10, 5, 0.1),
            (2, 100, 1, 0.1),
            (25, 25, 1, 0.1),
            (25,50,3,0.1),
            (25,50,3,0.3),
            (25,50,3,0.6),
            (25, 100, 1, 0.6),
            (25, 100, 5, 0.3),
            (25, 100, 5, 0.6),
            (50, 50, 1, 0.1),
            (50, 100, 1, 0.1),
        ]'''
    elif(table_type == "time"):
        header = grouping_header + ["$D_0$", "\\textbf{mean}", "\\textbf{std}"]
        columns = grouping + ['diversity', 'mean_generations_ratio', 'std_generations']
        decimal_columns += ['std_generations', 'mean_generations_ratio']
        highlight_cols = ['mean_generations_ratio']
        highlight_max = [False]
        highlight_colors = {'mean_generations_ratio': 'lightgray'}
        description = ['diversity', 'mean number of generations', 'standard deviation of number of generations']
    elif(table_type == "diversity"):
        header = grouping_header + ["$D_0$", "$D_{x}$"]
        columns = grouping + ['diversity', 'max_perc']
        decimal_columns += ['max_perc']
        highlight_cols = ['diversity']
        highlight_max = [True]
        highlight_colors = {'diversity': 'lightgray'}
        description = ['diversity', 'percentage of cases where diversity is max']
    elif(table_type == "robustness"):
        header = grouping_header + ["$D_0$", "$R_1$", "$R_2$"]
        columns = grouping + ['diversity', 'Perc_rob_test_0', 'Perc_rob_test_1']
        decimal_columns += ['Perc_rob_test_0', 'Perc_rob_test_1']
        highlight_cols, exclude_rows = ['Perc_rob_test_0', 'Perc_rob_test_1'], {"init": "True"}
        highlight_max = [True, True]
        highlight_colors = {'Perc_rob_test_0': 'lightgray', 'Perc_rob_test_1': 'gray'}
        description = ['diversity', 'percentage of successful tests with one constraint', 'percentage of successful tests with two constraints']
        
    else:
        print("Invalid table type")
        exit(1)
    
    if(table_type == "fitness" or table_type == "time" or table_type == "diversity"):
        csv_files = [            
            f"results/out_Mu1-{'' if constrained else 'un'}const_1RAI_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_0.100000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_0.200000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_2.000000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_NSWAP_summary.csv"    
        ]
    else:
        csv_files = [            
            f"results/out_rob_Mu1-{'' if constrained else 'un'}const_1RAI_summary.csv",
            f"results/out_rob_Mu1-{'' if constrained else 'un'}const_XRAI_0.100000_summary.csv",
            f"results/out_rob_Mu1-{'' if constrained else 'un'}const_XRAI_0.200000_summary.csv",
            f"results/out_rob_Mu1-{'' if constrained else 'un'}const_XRAI_2.000000_summary.csv",
            f"results/out_rob_Mu1-{'' if constrained else 'un'}const_NSWAP_summary.csv"    
        ]
    
    table = get_table(csv_files, header, columns, grouping, grouping_header, decimal_columns, double_decimal_columns, upper_header, filtered, highlight_cols, exclude_rows, highlight_max, highlight_colors, description)
    with open(outputfile, 'w') as f:
        f.write(table)