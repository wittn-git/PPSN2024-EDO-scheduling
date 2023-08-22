import sys
import pandas as pd

mus = [2,10,25,50]
ns = [5,10,25,50,100]
ms = [1,3,5,10]
alphas = [0.2,0.5,1]

def iterate_combinations(df, with_alphas, with_nswap, runs):
    count, count_missing = 0, 0
    for mu in mus:
        for n in ns:
            for m in ms:
                if(m >= n or (mu > (n*n - n)/(n-m))): continue
                if(with_nswap and m > 1): continue
                if(with_alphas):
                    for alpha in alphas:
                        count += 1
                        occurrences = 0
                        if(not df[(df['mu'] == mu) & (df['n'] == n) & (df['m'] == m) & (df['alpha'] == alpha)]['occurrences'].empty):
                            occurrences = df[(df['mu'] == mu) & (df['n'] == n) & (df['m'] == m) & (df['alpha'] == alpha)].iloc[0]['occurrences']
                        if(occurrences < runs):
                            count_missing += 1
                            print("mu: " + str(mu) + ", n: " + str(n) + ", m: " + str(m) + ", alpha: " + str(alpha) + ", occurrences: " + str(int(occurrences)) + "/" + str(runs))
                else:
                    count += 1
                    occurrences = 0
                    if(not df[(df['mu'] == mu) & (df['n'] == n) & (df['m'] == m)]['occurrences'].empty):
                        occurrences = df[(df['mu'] == mu) & (df['n'] == n) & (df['m'] == m)].iloc[0]['occurrences']
                    if(occurrences < runs):
                        count_missing += 1
                        print("mu: " + str(mu) + ", n: " + str(n) + ", m: " + str(m) + ", occurrences: " + str(int(occurrences)) + "/" + str(runs))
    print("Total combinations: " + str(count))
    print("Missing / incomplete combinations: " + str(count_missing))
    print()

def get_filtered_df(df, algorithm, mutation, grouping_columns):
    df.drop_duplicates()
    filtered_df = df[(df['algorithm'] == algorithm) & (df['mutation'] == mutation)]
    grouped = filtered_df.groupby(grouping_columns)
    grouped = grouped.size().reset_index(name='occurrences')
    return grouped
    
def print_missing_combinations(input_files, grouping_columns, runs):
    for input_file in input_files:
        df = pd.read_csv(input_file)
        algorithm_combinations = df[['algorithm', 'mutation']].drop_duplicates()
        for _, entry in algorithm_combinations.iterrows():
            grouped_df = get_filtered_df(df, entry['algorithm'], entry['mutation'], grouping_columns)
            with_alphas = "alpha" in grouping_columns
            with_nswap = entry['mutation'] == "NSWAP"
            print("Missing combinations for " + entry['algorithm'] + " with " + entry['mutation'] + ":")
            iterate_combinations(grouped_df, with_alphas, with_nswap, runs)

if __name__ == '__main__':

    if len(sys.argv) < 4:
        print("Usage: python3 AnalyzeData.py <input_file> <grouping_columns> <runs>")
        exit(1)

    input_files = sys.argv[1].split(",")
    grouping_columns = sys.argv[2].split(",")
    runs = int(sys.argv[3])

    print_missing_combinations(input_files, grouping_columns, runs)