# Python Scripts directory

This directory contains python scripts that automate various operations.

## [`runner_sequential.py`](./runner_sequential.py) and [`runner_parallel.py`](.//runner_parallel.py)

Builds and runs the executables of the code in [source](../source/) subfolders, for the sequential and parallel program.
Use `-h` flag to see the usage and the available command line arguments.

## [`results_sequential.py`](./results_sequential.py)

Runs the sequential program's executable for various command line arguments, gathers the execution time values, produces an average and
stores it in `.csv` files
with appropriate names in the [results](../results/sequential/) folder.
It performs checks on the directories and files and creates them if they do not exist
Use `-h` flag to see the usage and the available command line arguments.

## [`results_parallel.py`](./results_parallel.py)

Runs the parallel program's executable for various command line arguments, gathers the execution and data share time values, produces an average and
stores it in `.csv` files
with appropriate names in the [results](../results/parallel/) folder.
It performs checks on the directories and files and creates them if they do not exist
Use `-h` flag to see the usage and the available command line arguments.

## [`speedup_efficiency.py`](./speedup_efficiency.py)

From the result `.csv` files it produces the speedup and efficiency metrics and stores them in the [results](../results/parallel/) folder.

It can also run the [`results_sequential.py`](./results_sequential.py) and [`results_parallel.py`](./results_parallel.py) scripts first.

Use `-h` flag to see the usage and the available command line arguments.

## [`table_creator.py`](./table_creator.py)

Creates contents of the LaTeX tables that are shown in the final report.
The scaffold template of the table is in [latex_tables](../results/latex_tables/) direcotry. This script produces files in the afformantioned directory, to copy and paste their contents to the template and finally create the table.
Use `-h` flag to see the usage and the available command line arguments.