# Exercise Implementation

In this problem, the user must input the number if threads and a number of iterations

An array of integers with the size of the thread number is created. Each thread updates a single element of the array, by adding 1, as many times as the inputted iterations number. This creates false sharing between the processing cores.

In this exercise there are three implementations: one direct implementation of the problem and two solution-implementations that battle false sharing.
The first solution is to update each element and write back to the array, after computing the final value. This way there is race condition only in the final updates.
The second silution is spacing out the elements inside the array by adding junk values. This way elements appear with spacing in the size of the system's cache line and each thread can act on cache lines that do not need to be shared with other threads. To implement this the cache line size of the system is needed. The python scripts get that automatically.

The C programs of this exercise take as arguments the number if threads and number of iterations.
After they execute they print to the terminal the expected value of the common array variables after processing
and theit sum, as well as
their actual value and their sum.

Python scripts are used to automate the build process and
the execution for various thread counts and iterations.
They also check that the expected and the actual value of the common array elements and their sum is the same, for all the executions,
and they gather the timing results and the computed efficiencies in .csv files

# Folder Structure

- root folders
    - **'initial/'** has the code, build system and scripts for the direct implementation of the problem.
    - **'solution_1'** has the code, build system and scripts for the local variable solution (1st solution) the problem.
    - **'solution_2'** has the code, build system and scripts for the junk injection solution (2nd solution) the problem.
- internal structure of root folders
    - **'src/"** has all the C source code, which is the main code of the implementations
    - **'inc/"** all the included header files for the C source code
    - **'build/"** the folder will be created if you build the project. Inside it there is the executable named 'app'
    - **'results/'** created by the python script `results.py`. Inside it the .csv files with the timing and efficiency results, for various iterations and thread counts
        - **'latex_tables/'** contains the python script `table_creator.py` that creates latex table templates that where used in the report

## Files int the root folder

- `ex3_results_script.sh` is a bash script to execute the cripts that produce results for the three implementations. Its use is shown in the 'Build and Run' section
- `plots.py` creates a plot as a .pdf file in this root directory, which contains an illustration of the results for various thread counts and constant number of iterations at $10^6$. _You need to run `ex_2_results_script.sh` first!_
- `system_info.py` creates a .ini file with system information. There is also the `system_DI.ini` file which was produced from this script for the PC's that ran the results that are contained in the final report.


# Build and Run

## Build with make

The build system used is make.

To clean all the produced binaries run
```
make clean
```

To build the executable, which will be stored inside the 'build/' folder run
```
make run
```

To run the executable go to the 'build/' folder and run it.
See the needed arguments and add them accordingly (thread count and iterations).
The expected and the actual values of the common array are printed on the terminal
as well as the execution time
```
./app <threadnum> <iterations>
```

## Usage of scripts

The scripting is done with python programs inside every root folder.

To compile and run directly the program, run the following script
```
python3 runner.py <threadnum> <iterations>
```

To produce the results automatically run the following script.
This one runs the `runner.py` script for various arguments and captures the output.
More specifically it runs it for $10^1$ up to $10^6$ iterations
for thread numbers ranging from $2^0=1$ thread up to $2^5=32$ threads.
For every execution of the `runner.py` the console output is captured
the expected values are compared to the actual ones
and the execution time along with the calculated efficiencies
are stored in .csv files in the 'results/' folder.
If an expected value is different than the actual value then 'None' is stored in the corresponding .csv slot
```
python3 results.py
```

The `ex3_results_script.sh` runs all the `results.py` scripts inside the root folders