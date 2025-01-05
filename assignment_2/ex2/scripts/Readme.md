# Python Scripts directory

This directory contains python scripts that automate various operations.

## [`systeminfo.py`](./systeminfo.py)

Creates a `.ini` file with information about the current system in use.

## [`runner.py`](./runner.py)

Builds and runs the executables of the code in [src](../src/) folder with the header files in [inc](../inc/) folder.
Use `-h` flag to see the usage and the available command line arguments.

## [`results.py`](./results.py)

Runs the executables for various command line arguments, gathers the speedup values and produces averages of them. The results are stored in `.csv` files
with appropriate names in the [results](../results/) folder.
Use `-h` flag to see the usage and the available command line arguments.

## [`table_creator.py`](./table_creator.py)

Creates contents of the LaTeX tables that are shown in the final report.
The scaffold template of the table is in [latex_tables](../results/latex_tables/) direcotry. This script produces files in the afformantioned directory, to copy and paste their contents to the template and finally create the table.
Use `-h` flag to see the usage and the available command line arguments.