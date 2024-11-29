#! ./env/bin/python3

import os
import re
import conf
import time
import argparse
import numpy as np
import pandas as pd
import subprocess
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

# Set matplotlib font
if conf.font_path != "":
    font_manager.fontManager.addfont(conf.font_path)
    prop = font_manager.FontProperties(fname=conf.font_path)

    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = prop.get_name()
    plt.rcParams.update({'font.size': 11})


# Extract information from program's output
time_regex = re.compile(r"Elapsed time = (\d+\.\d+) seconds")
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
    thread_nums: list[int],
    configurations: list[dict[str, int]],
    file_path: str
):
    """Run the program with different configurations and save the data to a csv file.
    An example of a configuration dictionary is listed below:
    {
        "initial_keys": 1000,
        "operations_num": 500000,
        "search_presentage": 0.99,
        "insert_presentage": 0.005,
        "delete_presentage": 0.005
    }.

    Args:
        iterations (int): The number of times to run the program with the same configuration.
        thread_nums (list[int]): The number of threads to run the program with.
        configurations (list[dict[str, int]]): The configurations to run the program with.
        file_path (str): The path to the csv file to save the data.
    """
    # Header of the csv file
    with open(file_path, "w") as f:
        f.write(
            "threads;"
            "initial_keys;"
            "operations_num;"
            "search_presentage;"
            "insert_presentage;"
            "delete_presentage;"
            "time(s);"
            "\n"
        )

    for config in configurations:
        for thread_num in thread_nums:
            for _ in range(iterations):
                # Execute program and catch output
                output = subprocess.run(
                    [
                        f"{root_ex_folder}/bin/main",
                        "-t", f"{thread_num}",
                        "-k", f"{config['initial_keys']}",
                        "-o", f"{config['operations_num']}",
                        "-s", f"{config['search_presentage']}",
                        "-i", f"{config['insert_presentage']}",
                        "-d", f"{config['delete_presentage']}",
                    ],
                    capture_output=True,
                )

                output = output.stdout.decode('utf-8')
                output = output.split("\n")
                output = output[:-1]  # remove last empty line

                # Create csv file with data from execution
                with open(file_path, "a") as f:
                    # Theads and configuration
                    f.write(f"{thread_num};")
                    f.write(f"{config['initial_keys']};")
                    f.write(f"{config['operations_num']};")
                    f.write(f"{config['search_presentage']};")
                    f.write(f"{config['insert_presentage']};")
                    f.write(f"{config['delete_presentage']};")

                    for line in output:
                        if time_regex.search(line):
                            time = time_regex.search(line).group(1)
                            # Execution time
                            f.write(f"{time}\n")


def plot_data(
    read_data_path: str
):
    """Plot data from csv file. Each line of the csv file should have the following format:
    threads;initial_keys;operations_num;search_presentage;insert_presentage;delete_presentage;time(s). The first line is the header.

    Args:
        read_data_path (str): The path to the csv file with the data.
    """
    data = pd.read_csv(
        read_data_path,
        delimiter=";"
    )
    data = data.drop(
        columns=[
            "initial_keys",
            "operations_num",
            "insert_presentage",
            "delete_presentage"
        ]
    )

    data_grouped = data.groupby(["threads", "search_presentage"]).agg(
        time=("time(s)", "mean"),
        time_max=("time(s)", "max"),
        time_min=("time(s)", "min")
    )

    # Plot Data
    fig, ax = plt.subplots()

    textwidth_latex = 398.33858
    fig_size_inches = textwidth_latex / 72
    golden_ratio = (1 + np.sqrt(5)) / 2
    fig.set_size_inches(fig_size_inches, fig_size_inches / golden_ratio)

    search_presentages = data_grouped.index.levels[1]

    for search_presentage in search_presentages:
        data_plot = data_grouped.reset_index()
        data_plot = data_plot[data_plot["search_presentage"]
                              == search_presentage]

        yerr_min = np.absolute(data_plot["time"] - data_plot["time_min"])
        yerr_max = np.absolute(data_plot["time"] - data_plot["time_max"])
        yerr = np.stack([yerr_min.values, yerr_max.values])

        ax.errorbar(
            x=data_plot["threads"],
            y=data_plot["time"],
            label=f"{search_presentage * 100}% Member",
            ms=4,
            yerr=yerr,
            fmt="o",
            elinewidth=1.2,
            capthick=1.2,
            capsize=2.4,
        )

    ax.set_xlabel("Threads")
    ax.set_ylabel("Time (s)")
    ax.legend()

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

        file_path = f"{save_plot_dir}/{time}.pdf"
        fig.savefig(file_path, format="pdf")
    else:
        print("[ERROR] Could not extract timestamp from file path")


def results_to_latex(
    read_data_path: str
):
    """Save the data from the csv file to a latex table.
    The mean time is used to caclulate the speedup and efficiency.

    Args:
        read_data_path (str): The path to the csv file with the data.
    """
    data = pd.read_csv(
        read_data_path,
        delimiter=";"
    )

    data_grouped = data.groupby(
        data.keys()[0:-1].tolist()
    ).agg(
        time=("time(s)", "mean")
    ).reset_index().sort_values(
        by=["search_presentage", "threads"]
    )

    timestamp = timestamp_from_path.search(read_data_path)
    if timestamp:
        date = timestamp.group(1)
        time = timestamp.group(2)

        save_table_dir = f"{root_data_table_folder}/{date}"
        os.makedirs(save_table_dir, exist_ok=True)

        file_path = f"{save_table_dir}/{time}.tex"
        latex_table = data_grouped.to_latex(
            index=False,
            column_format=data_grouped.columns.size * "c",
            header=[col.capitalize() for col in data_grouped.columns],
        )
        with open(file_path, "w") as f:
            f.write(latex_table)
    else:
        print("[ERROR] Could not extract timestamp from file path")


# Parse arguments by creatting a parser
parser = argparse.ArgumentParser(
    description="Run program with different configuration, save data to csv file and produce tables.",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

parser.add_argument(
    "--execute",
    type=int,
    default=0,
    help="Execute the program with different configuration, plot the data and create LaTeX table."
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
        thread_nums = [1, 2, 3, 4, 5, 6, 7, 8]

        initial_keys = 1000
        operations_num = 500000
        configurations = [
            {
                "initial_keys": initial_keys,
                "operations_num": operations_num,
                "search_presentage": 0.999,
                "insert_presentage": 0.0005,
                "delete_presentage": 0.0005
            },
            {
                "initial_keys": initial_keys,
                "operations_num": operations_num,
                "search_presentage": 0.95,
                "insert_presentage": 0.025,
                "delete_presentage": 0.025
            },
            {
                "initial_keys": initial_keys,
                "operations_num": operations_num,
                "search_presentage": 0.90,
                "insert_presentage": 0.05,
                "delete_presentage": 0.05
            }
        ]

        exec_file_path = f"{save_exec_folder}/{time_string}.csv"

        run_programs(iterations, thread_nums, configurations, exec_file_path)

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
