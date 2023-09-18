import pandas as pd
import sys

def summarize_trajectory(input_file, output_file, grouping_columns):
    df = pd.read_csv(input_file)
    grouping_columns.append('generations')
    grouped = df.groupby(grouping_columns)['diversity'].mean().reset_index()
    grouped.to_csv(output_file, index=False)


if(__name__ == "__main__"):

    if len(sys.argv) < 4:
        print("Usage: python3 SummarizeTrajectory.py <input_file> <output_file> <grouping_columns>")
        exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    grouping_columns = sys.argv[3].split(",")

    summarize_trajectory(input_file, output_file, grouping_columns)