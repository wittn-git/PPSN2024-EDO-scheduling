import sys
import os
import glob

def process_files(prefix, input_folder, outputfile):
    file_pattern = os.path.join(input_folder, f"{prefix}*")
    file_list = glob.glob(file_pattern)

    if not file_list:
        print("No files found.")
        return

    with open(outputfile, 'w') as output:
        with open(file_list[0], 'r') as file:
            lines = file.readlines()
            output.writelines(lines[0])
        for file_path in file_list:
            with open(file_path, 'r') as file:
                lines = file.readlines()
                output.writelines(lines[1:])
    print("Processing complete.")

if(__name__ == "__main__"):

    if len(sys.argv) < 4:
        print("Usage: python3 ConcatFiles.py <input_folder> <prefix> <output_file_name>")
        exit(1)

    input_folder = sys.argv[1]
    prefix = sys.argv[2]
    output_file = sys.argv[3]

    process_files(prefix, input_folder, output_file)