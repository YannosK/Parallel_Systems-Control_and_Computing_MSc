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

