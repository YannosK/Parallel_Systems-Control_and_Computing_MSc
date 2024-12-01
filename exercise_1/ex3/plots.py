import conf
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager
import pandas as pd
import numpy as np

# Set matplotlib font
font_manager.fontManager.addfont(conf.font_path)
prop = font_manager.FontProperties(fname=conf.font_path)

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = prop.get_name()
plt.rcParams.update({'font.size': 11})

def plot_data():
    """
    Plot efficiency data for the 1000000 iterations from three CSV files
    and save the plot as a fixed PDF file name.
    """
    ######################################
    # read .csv files
    ######################################
    initial_csv = "./initial/results/efficiencies.csv"
    solution1_csv = "./solution_1/results/efficiencies.csv"
    solution2_csv = "./solution_2/results/efficiencies.csv"

    initial_data = pd.read_csv(initial_csv)
    solution1_data = pd.read_csv(solution1_csv)
    solution2_data = pd.read_csv(solution2_csv)

    # Extract data for 1000000 iterations
    initial_row = initial_data[initial_data["Iterations"] == 1000000].iloc[0]
    solution1_row = solution1_data[solution1_data["Iterations"] == 1000000].iloc[0]
    solution2_row = solution2_data[solution2_data["Iterations"] == 1000000].iloc[0]

    # Prepare thread counts and times
    threads = [int(col.split()[0]) for col in initial_data.columns[1:]]  # Extract thread counts
    initial_efficiencies= initial_row[1:].values
    solution1_efficiencies= solution1_row[1:].values
    solution2_efficiencies= solution2_row[1:].values

    ######################################
    # plot data
    ######################################
    fig, ax = plt.subplots()

    # Set figure size based on golden ratio
    textwidth_latex = 398.33858
    fig_size_inches = textwidth_latex / 72
    golden_ratio = (1 + np.sqrt(5)) / 2
    fig.set_size_inches(fig_size_inches, fig_size_inches / golden_ratio)

    # Plot data
    ax.plot(threads, initial_efficiencies, label="Initial", marker="o", color="blue", markersize=3, linewidth=0.6)
    ax.plot(threads, solution1_efficiencies, label="Solution 1", marker="s", color="orange", markersize=3, linewidth=0.6)
    ax.plot(threads, solution2_efficiencies, label="Solution 2", marker="^", color="green", markersize=3, linewidth=0.6)

    ######################################
    # plot parameters
    ######################################
    ax.set_xlabel("Threads")
    ax.set_ylabel("Efficiency")
    # ax.set_title("Thread Times for 1000000 Iterations")
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

    ######################################
    # save plot to file
    ######################################
    file_path = "ex3_plot.pdf"
    fig.savefig(file_path, format="pdf")
    print(f"Plot saved to {file_path}")

    ######################################
    # Show plot
    ######################################
    plt.show()

if __name__ == "__main__":
    plot_data()
