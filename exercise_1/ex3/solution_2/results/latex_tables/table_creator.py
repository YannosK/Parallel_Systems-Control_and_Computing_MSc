import csv
import sys

def csv_to_list(filename):
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        return [row for row in reader]
    
def string_to_power_of_ten(value: str) -> str:
    if not value.isdigit() or not value.startswith("1") or any(c != '0' for c in value[1:]):
        raise ValueError("Input must be a string like '10', '100', '1000', etc.")
    exponent = len(value) - 1
    return f"$10^{exponent}$"

if __name__ == "__main__":

    ###################################
    # Argument check
    ###################################

    if len(sys.argv) != 2:
        print('Usage: python3 runner.py <tabletype>')
        print('Table type is either tim for timings or ef for efficiencies')
        sys.exit(1)

    tabletype = sys.argv[1]

    if tabletype == 'tim':
        csv_file = "../timings.csv"
    elif tabletype == 'ef':
        csv_file = "../efficiencies.csv"
    else:
        raise ValueError('Table type is either tim for timings or ef for efficiencies')

    ###################################
    # Csv to lists
    ###################################

    valuelist = csv_to_list(csv_file)

    ###################################
    # Creating tex blocks
    ###################################

    # LaTeX template for a single block
    template_block = """
    %%%%%%%%%%%%%%%%%%
    {Iterations}
    % 1 Thread 
    & {Val_1}
    % 2 Threads 
    & {Val_2}
    % 4 Threads 
    & {Val_4}
    % 8 Threads 
    & {Val_8}
    \\\\
"""

    if tabletype == 'tim':
        output_filename = "timings_template.tex"
    elif tabletype == 'ef':
        output_filename = "efficiencies_template.tex"
    
    with open(output_filename, "w") as tex_file:
        for value_row in valuelist:
            tex_block = template_block.format(
                # Iterations=string_to_power_of_ten(value_row[0]), # choose this for exponent notation
                Iterations=value_row[0], # choose this for full values
                Val_1="{:.7f}".format(float(value_row[1])),
                Val_2="{:.7f}".format(float(value_row[2])),
                Val_4="{:.7f}".format(float(value_row[3])),
                Val_8="{:.7f}".format(float(value_row[4]))
            ).strip()  # Remove extra newlines at the start and end of the block
            tex_file.write("\t" + tex_block + "\n")

    print(f"LaTeX file '{output_filename}' generated successfully.")
