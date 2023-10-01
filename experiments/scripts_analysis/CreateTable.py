import pandas as pd
import sys

def get_table(csv_file, shown_header, actual_header, grouping_attributes, grouping_header, decimal_columns, upper_header, filtered_mus, include_entries=[], scriptsize=True):
    dataframes = [pd.read_csv(file) for file in csv_file]
    
    for df in dataframes:
        df = df.sort_values(by=grouping_attributes)
        df = df[df['mu'].isin(filtered_mus)]

    if(include_entries):
        for i, df in enumerate(dataframes):
            dataframes[i] = df.loc[include_entries]

    for column in actual_header:
        for i, df in enumerate(dataframes):
            if(column in decimal_columns):
                df[column] = df[column].apply(lambda x: f"{x:.2f}")
            else:
                df[column] = df[column].apply(lambda x: f"{x:.0f}")

    ungrouped_header = [x for x in shown_header if x not in grouping_header]

    latex_table = "\\begin{center}\n"
    latex_table += "\\renewcommand{\\tabcolsep}{4pt}\n"
    latex_table += " \\renewcommand{\\arraystretch}{1.1}\n"
    if scriptsize: latex_table += " \\begin{scriptsize}\n"
    latex_table += f" \\begin{{{'tabular'}}}{{{'r'*(len(grouping_attributes)+len(ungrouped_header) * len(dataframes))}}}\n"
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

    for row in dataframes[0].iterrows():
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
                    occ_keys.append(currents[attr])
                    if(attr == attribute): break
                columns.append(f"\multirow{{{occurence_dict[','.join(occ_keys)]}}}{{{'*'}}}{{{first_row[attribute]}}}") 
            else: columns.append("")
        for df in dataframes:
            for attribute in actual_header: 
                if(attribute not in currents):
                    row_ = df
                    for attr, val in currents.items():
                        row_ = row_[row_[attr] == val]
                    if(row_.empty): 
                        columns.append("---")
                    else:
                        columns.append(row_[attribute].values[0])

        latex_table += " & ".join(columns) + "\\\\ \n"
    
    latex_table += " \\bottomrule\n"
    latex_table += "  \end{tabular}\n"
    if scriptsize: latex_table += " \end{scriptsize}\n"
    latex_table += " \end{center}\n"
    return latex_table

def get_running_time_table():
    pass

def get_robustness_table():
    pass

if __name__ == "__main__":

    if len(sys.argv) < 4:
        print("Usage: python3 CreateTable.py <table_type> <constrained> <outputfile> [<filtered_mus>] [<include_entries>]")
        exit(1)

    table_type = sys.argv[1]
    constrained = sys.argv[2] == "True"
    outputfile = sys.argv[3]
    include_entries = []
    if(len(sys.argv) > 4):
        filtered_mus = [int(x) for x in sys.argv[4].split(",")]
    if(len(sys.argv) > 5):
        include_entries = [int(x) for x in sys.argv[4].split(",")]

    upper_header = ["$1(R+I)$", "$X(R+I), \lambda = 0.1$", "$X(R+I), \lambda = 0.2$", "$X(R+I), \lambda = 2.00$", "$NSWAP$"]
    grouping = ['mu', 'n', 'm']
    grouping_header = ["$\mu$", "$n$", "$m$"]
    decimal_columns = ['diversity']
    if(constrained):
        grouping.append('alpha')
        grouping_header.append("$\\alpha$")
        decimal_columns.append('alpha')
    
    if(table_type == "fitness"):
        header = grouping_header + ["$D_0$", "$OBJ$", "$OPT$"]
        columns = grouping + ['diversity', 'fitness', 'opt']
        decimal_columns += ['fitness', 'opt']
    elif(table_type == "time"):
        header = grouping_header + ["$D_0$", "\\textbf{mean}", "\\textbf{std}"]
        columns = grouping + ['diversity', 'generations', 'std_generations']
        decimal_columns += ['std_generations', 'generations']
    elif(table_type == "diversity"):
        header = grouping_header + ["$D_0$", "$Dx$"]
        columns = grouping + ['diversity', 'diversity_not_1']
        decimal_columns += ['diversity_not_1']
    elif(table_type == "robustness"):
        pass
    else:
        print("Invalid table type")
        exit(1)
    
    if(table_type == "fitness" or table_type == "running_time" or table_type == "diversity"):
        csv_files = [            
            f"results/out_Mu1-{'' if constrained else 'un'}const_1RAI_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_0.100000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_0.200000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_XRAI_2.000000_summary.csv",
            f"results/out_Mu1-{'' if constrained else 'un'}const_NSWAP_summary.csv"    
        ]
    else:
        pass
    
    table = get_table(csv_files, header, columns, grouping, grouping_header, decimal_columns, upper_header, filtered_mus, include_entries)
    with open(outputfile, 'w') as f:
        f.write(table)
    
    # TODO make robustness table