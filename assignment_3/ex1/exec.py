import os
import re
import time
import argparse
import itertools
import subprocess
import numpy as np
import pandas as pd
from typing import List
from dotenv import load_dotenv
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

# Set the environment variables
load_dotenv()
FONT_PATH = os.getenv("FONT_PATH")

# Set matplotlib font
if FONT_PATH != "":
    font_manager.fontManager.addfont(FONT_PATH)
    prop = font_manager.FontProperties(fname=FONT_PATH)

    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = prop.get_name()
    plt.rcParams.update({'font.size': 11})


# Extract information from program's output
time_regex = re.compile(r"Execution time: (\d+\.\d+)s")
timestamp_from_path = re.compile(r"(\d{4}-\d{2}-\d{2})/(\d{2}-\d{2}-\d{2})")


# Folder paths
root_ex_folder = os.path.abspath(
    os.path.join(os.path.realpath(__file__), '..')
)
root_data_folder = f"{root_ex_folder}/data"
root_data_exec_folder = f"{root_data_folder}/exec"
root_data_plot_folder = f"{root_data_folder}/plot"
root_data_table_folder = f"{root_data_folder}/table"


def run_programs(
    iterations: int,
    generations_list: List[int],
    grid_list: List[int],
    processes_list: List[int],
    file_path: str
):
    """Run program with different generations, grid and processes and
    save data to csv file. The execution is repeated for a number of iterations.

    Args:
        iterations (int): Number of iterations to repeat the execution.
        generations_list (List[int]): List of generations to run the program with.
        grid_list (List[int]): List of grid sizes to run the program with .
        processes_list (List[int]): List of processes numbers to run the program with.
        file_path (str): The path to the csv file to save the data.
    """
    with open(file_path, "w") as f:
        f.write(f"iteration;generations;grid;processes;time(s)\n")

    for iteration in range(iterations):
        for element in itertools.product(generations_list, grid_list, processes_list):
            # Execute program and catch output
            output = subprocess.run(
                [
                    f"mpiexec",
                    f"-f",
                    f"machines",
                    f"-n",
                    f"{element[2]}",
                    f"{root_ex_folder}/bin/main",
                    f"{element[0]}",
                    f"{element[1]}",
                ],
                capture_output=True,
            )

            output = output.stdout.decode('utf-8')
            output = output.split("\n")
            output = output[:-1]  # remove last empty line

            # Create csv file with data from execution
            with open(file_path, "a") as f:
                f.write(f"{iteration};")
                f.write(f"{element[0]};")
                f.write(f"{element[1]};")
                f.write(f"{element[2]};")

                for line in output:
                    if time_regex.search(line):
                        time = time_regex.search(line).group(1)
                        f.write(f"{time}")

                f.write("\n")


def plot_data(
    read_data_path: str
):
    """Plot data from csv file. Each line of the csv file should have the following format:
    iteration;generation;grid;processes;time(s). The first row should be the header.

    Args:
        read_data_path (str): The path to the csv file with the data.
    """
    data = pd.read_csv(read_data_path, delimiter=";")
    data_grouped = data.groupby(["grid", "processes"]).agg(
        time=("time(s)", "mean"),
        time_max=("time(s)", "max"),
        time_min=("time(s)", "min")
    )

    for grid in data_grouped.index.levels[0]:
        # Plot Data
        fig, ax = plt.subplots()

        textwidth_latex = 398.33858
        fig_size_inches = textwidth_latex / 72
        golden_ratio = (1 + np.sqrt(5)) / 2
        fig.set_size_inches(fig_size_inches, fig_size_inches / golden_ratio)

        data_grid = data_grouped.loc[grid]

        yerr_min = np.absolute(data_grid["time"] - data_grid["time_min"])
        yerr_max = np.absolute(data_grid["time"] - data_grid["time_max"])
        yerr = np.stack([yerr_min.values, yerr_max.values])

        ax.errorbar(
            x=data_grid.index,
            y=data_grid["time"],
            ms=4,
            yerr=yerr,
            fmt="o",
            elinewidth=1.2,
            capthick=1.2,
            capsize=2.4,
        )

        ax.set_xlabel("Processes")
        ax.set_ylabel("Time (s)")

        ax.grid(
            visible=True,
            which='major',
            color='#777777',
            linestyle='--',
            linewidth=0.8
        )
        ax.minorticks_on()
        ax.grid(
            visible=True,
            which='minor',
            color='#999999',
            linestyle='--',
            alpha=0.2,
            linewidth=0.8
        )

        fig.tight_layout()

        # Save plot to file
        timestamp = timestamp_from_path.search(read_data_path)
        if timestamp:
            date = timestamp.group(1)
            time = timestamp.group(2)

            save_plot_dir = f"{root_data_plot_folder}/{date}"
            os.makedirs(save_plot_dir, exist_ok=True)

            file_path = f"{save_plot_dir}/{time}-{grid}.pdf"
            fig.savefig(file_path, format="pdf")
        else:
            print("[ERROR] Could not extract timestamp from file path")


def results_to_latex(
    read_data_path: str
):
    """Save the data from the csv file to a latex table.
    The mean time is used to calculate the speedup and efficiency.

    Args:
        read_data_path (str): The path to the csv file with the data.
    """
    data = pd.read_csv(read_data_path, delimiter=";")
    data_grouped = data.groupby(["grid", "processes"]).agg(
        time=("time(s)", "mean"),
    )

    for grid in data_grouped.index.levels[0]:
        data_grouped_grid = data_grouped.loc[grid]

        serial_time = data_grouped_grid["time"].iloc[0]
        data_grouped_grid["speedup"] = serial_time / data_grouped_grid["time"]

        number_of_processes = data_grouped_grid.index
        data_grouped_grid["efficiency"] = data_grouped_grid["speedup"] / \
            number_of_processes

        # Convert indexes to columns
        data_grouped_grid = data_grouped_grid.reset_index()
        # Sort values
        data_grouped_grid = data_grouped_grid.sort_values(
            ["processes", "time"])

        latex_table = data_grouped_grid.to_latex(
            index=False,
            column_format=data_grouped_grid.columns.size * "c",
            header=[col.capitalize() for col in data_grouped_grid.columns],
        )

        timestamp = timestamp_from_path.search(read_data_path)
        if timestamp:
            date = timestamp.group(1)
            time = timestamp.group(2)

            save_table_dir = f"{root_data_table_folder}/{date}"
            os.makedirs(save_table_dir, exist_ok=True)

            file_path = f"{save_table_dir}/{time}-{grid}.tex"
            with open(file_path, "w") as f:
                f.write(latex_table)
        else:
            print("[ERROR] Could not extract timestamp from file path")


# Parse arguments by creating a parser
parser = argparse.ArgumentParser(
    description="Run program with different generations, grid and processes and, save data to csv file and produce tables.",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

parser.add_argument(
    "--execute",
    type=int,
    default=0,
    help="Execute the parallel program in MPI, plot the data and create LaTeX table."
)
parser.add_argument(
    "--plot",
    type=str,
    default="",
    help="Plot the data from the csv file."
)
parser.add_argument(
    "--table",
    type=str,
    default="",
    help="Print the table in latex format."
)

if __name__ == '__main__':
    args = parser.parse_args()

    if (args.execute == 1):
        # Get time stamp
        named_tuple = time.localtime()
        date_string = time.strftime("%Y-%m-%d", named_tuple)
        time_string = time.strftime("%H-%M-%S", named_tuple)

        save_exec_folder = f"{root_data_exec_folder}/{date_string}"
        os.makedirs(save_exec_folder, exist_ok=True)

        print("[INFO] Running programs")

        iterations = 10
        generations_list = [1e3]
        grid_list = [128, 1024, 8192]
        processes_list = [1, 4, 16, 64, 128]

        exec_file_path = f"{save_exec_folder}/{time_string}.csv"

        run_programs(
            iterations,
            generations_list,
            grid_list,
            processes_list,
            exec_file_path
        )

        print("[INFO] Plotting data")
        plot_data(exec_file_path)

        print("[INFO] Printing table")
        results_to_latex(exec_file_path)
    else:
        if (args.plot != ""):
            print("[INFO] Plotting data")
            exec_file_path = f"{root_data_exec_folder}/{args.plot}"
            plot_data(exec_file_path)

        if (args.table != ""):
            print("[INFO] Printing table")
            exec_file_path = f"{root_data_exec_folder}/{args.table}"
            results_to_latex(exec_file_path)
