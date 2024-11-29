import csv

def csv_to_list(filename):
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        return [row for row in reader]

if __name__ == "__main__":

    ###################################
    # Csv files
    ###################################

    timings_csv = "../timings.csv"
    efficiencies_csv = "../efficiencies.csv"

    ###################################
    # Csv to lists
    ###################################

    timings = csv_to_list(timings_csv)
    efficiencies = csv_to_list(efficiencies_csv)

    ###################################
    # Assertions
    ###################################

    assert len(timings) == len(efficiencies), "Mismatch in the number of rows between timings and efficiencies"
    for i in range(len(timings)):
        assert timings[i][0] == efficiencies[i][0], f"Iteration mismatch at row {i+1}"

    ###################################
    # Creating tex blocks
    ###################################

    # LaTeX template for a single block
    template_block = """
    %%%%%%%%%%%%%%%%%%
    {Iterations}
    % 1 Thread 
    & {Time_1} & {Eff_1}
    % 2 Threads 
    & {Time_2} & {Eff_2}
    % 4 Threads 
    & {Time_4} & {Eff_4}
    % 8 Threads 
    & {Time_8} & {Eff_8}
    \\\\
"""

    # Open a new LaTeX file to write the content
    output_filename = "generated_template.tex"
    with open(output_filename, "w") as tex_file:
        # Generate and write each block
        for timing_row, efficiency_row in zip(timings, efficiencies):
            tex_block = template_block.format(
                Iterations=timing_row[0],
                Time_1=timing_row[1], Eff_1=efficiency_row[1],
                Time_2=timing_row[2], Eff_2=efficiency_row[2],
                Time_4=timing_row[3], Eff_4=efficiency_row[3],
                Time_8=timing_row[4], Eff_8=efficiency_row[4],
            ).strip()  # Remove extra newlines at the start and end of the block
            tex_file.write(tex_block + "\n")

    print(f"LaTeX file '{output_filename}' generated successfully.")
