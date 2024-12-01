import matplotlib.pyplot as plt


def plot_data():
    """
    Plot time data from csv files for mutex lock and atomic operations implementation
    """

    ######################################
    # read .csv files
    ######################################

    mutex_csv = ""
    atomic_csv = ""

    data = pd.read_csv(read_data_path, delimiter=";")
    data_grouped = data.groupby(["mode", "threads"]).agg(
        time=("time(s)", "mean"),
        time_max=("time(s)", "max"),
        time_min=("time(s)", "min")
    )

    # Plot Data
    fig, ax = plt.subplots()

    textwidth_latex = 398.33858
    fig_size_inches =  textwidth_latex / 72
    golden_ratio = (1 + np.sqrt(5)) / 2
    fig.set_size_inches(fig_size_inches, fig_size_inches / golden_ratio)

    for mode in data_grouped.index.levels[0]:
        data_plot = data_grouped.loc[mode]

        yerr_min = np.absolute(data_plot["time"] - data_plot["time_min"])
        yerr_max = np.absolute(data_plot["time"] - data_plot["time_max"])
        yerr = np.stack([yerr_min.values, yerr_max.values])

        ax.errorbar(
            x=data_plot.index,
            y=data_plot["time"],
            label=mode.capitalize(),
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



if __name__ == "__main__":

    ##############################
    # 
    ##############################

    print()
