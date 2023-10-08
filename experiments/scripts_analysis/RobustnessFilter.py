import sys
import pandas as pd

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Usage: python3 RobustnessFilter.py <constrained>")
        exit(1)
    
    constrained = sys.argv[1] == "True"

    input_file = f"output_mu1-{'' if constrained else 'un'}const_robustness.csv"
    output_file = f"output_mu1-{'' if constrained else 'un'}const_robustness_shortend.csv"

    df = pd.read_csv(input_file)
    df = df.drop_duplicates()
    df = df.drop(columns=['rob_test_2','rob_test_3','rob_test_4','rob_test_5','rob_test_6','rob_test_7'])
    df = df[df['m'] == 1]
    df.to_csv(output_file, index=False)