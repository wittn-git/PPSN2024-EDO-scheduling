import pandas as pd

def read_csv_to_latex(csv_file, shown_header, actual_header, grouping_attributes, grouping_header, decimal_columns, upper_header, include_entries=[], footnotesize=True):
    dataframes = [pd.read_csv(file) for file in csv_file]

    for df in dataframes:
        df = df.sort_values(by=grouping_attributes)

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
    if footnotesize: latex_table += " \\begin{footnotesize}\n"
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
    # TODO fix inclusion of not existing attributes

    for rows in zip(*[df.iterrows() for df in dataframes]):
        first_row = rows[0][1]
        columns = []
        new_group = False
        for attribute in actual_header:
            if(attribute not in currents):
                for row in rows:
                    columns.append(row[1][attribute])
                continue
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
        latex_table += " & ".join(columns) + "\\\\ \n"
    
    latex_table += " \\bottomrule\n"
    latex_table += "  \end{tabular}\n"
    if footnotesize: latex_table += " \end{footnotesize}\n"
    latex_table += " \end{center}\n"
    return latex_table

if __name__ == "__main__":

    table_name = "table_unconstrained.tex"
    
    csv_files = [
        "results/out_Mu1-const_1RAI_summary.csv", 
        "results/out_Mu1-const_XRAI_0.000000_summary.csv",
        "results/out_Mu1-const_XRAI_0.250000_summary.csv",
        "results/out_Mu1-const_XRAI_1.000000_summary.csv",
        "results/out_Mu1-const_NSWAP_summary.csv",
    ]
    upper_header = ["$NSWAP$", "$1(R+I)$", "$X(R+I), \lambda = 0.00$", "$X(R+I), \lambda = 0.25$", "$X(R+I), \lambda = 1.00$"]
    header = ["$\mu$", "$n$", "$m$", "$\\alpha$", "$D_0$", "$OBJ$", "$OPT$", "$>\%$"]
    columns = ['mu', 'n', 'm', 'alpha', 'diversity', 'fitness', 'opt', 'fitness_worse_than_opt']
    grouping = ['mu', 'n', 'm', 'alpha']
    grouping_header = ["$\mu$", "$n$", "$m$", "$\\alpha$"]
    decimal_columns = ['diversity', 'fitness', 'alpha', 'opt', 'fitness_worse_than_opt']
    #include_entries = [2,3,5,9,11,12,16,17,31,32,33,62,63,72,73,101]
    
    latex_table = read_csv_to_latex(csv_files, header, columns, grouping, grouping_header, decimal_columns, upper_header, [], True)
    with open("results/" + table_name, 'w') as f:
        f.write(latex_table)