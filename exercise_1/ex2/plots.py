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
    Plot time data for the 1000000 iterations from two CSV files
    and save the plot as a fixed PDF file name.
    """
    ######################################
    # read .csv files
    ######################################
    mutex_csv = "./mutex_lock/results/timings.csv"
    atomic_csv = "./atomic_operations/results/timings.csv"

    mutex_data = pd.read_csv(mutex_csv)
    atomic_data = pd.read_csv(atomic_csv)

    # Extract data for 1000000 iterations
    mutex_row = mutex_data[mutex_data["Iterations"] == 1000000].iloc[0]
    atomic_row = atomic_data[atomic_data["Iterations"] == 1000000].iloc[0]

    # Prepare thread counts and times
    threads = [int(col.split()[0]) for col in mutex_data.columns[1:]]  # Extract thread counts
    mutex_times = mutex_row[1:].values
    atomic_times = atomic_row[1:].values

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
    ax.plot(threads, mutex_times, label="Mutex Lock", marker="o", color="blue", markersize=3, linewidth=0.6)
    ax.plot(threads, atomic_times, label="Atomic Operations", marker="s", color="orange", markersize=3, linewidth=0.6)

    ######################################
    # plot parameters
    ######################################
    ax.set_xlabel("Threads")
    ax.set_ylabel("Time (s)")
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
    file_path = "ex2_plot.pdf"
    fig.savefig(file_path, format="pdf")
    print(f"Plot saved to {file_path}")

    ######################################
    # Show plot
    ######################################
    plt.show()

if __name__ == "__main__":
    plot_data()
