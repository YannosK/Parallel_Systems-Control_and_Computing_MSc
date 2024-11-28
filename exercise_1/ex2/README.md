# Exercise Implementation

In this problem, the user must input the number of threads and a number of iterations

A common variable is updated by threads inputted by the user. The value of the variable is initialized to zero and each thread executes a loop that adds one to the variable. The result value must be deterministic and equal to _iterations_ * _threads_.
For this exercise there are two available implementations:
on is using pthread mutex locks and the other uses atomic operations of the GCC compiler.

The C programs of this exercise take as arguments the number if threads and number of iterations.
After they execute they print to the terminal the expected value of the common variable after processing and its actual value.
They also computes the execution time.

Python scripts are used to automate the build process and
the execution for various thread counts and iterations.
They also check that the expected and the actual value of the common variable is the same, for all the executions,
and they gather the timing results and the computed efficiencies in .csv files

# Folder Structure

- root folders
    - **'mutex_lock/'** has the code, build system and scripts for the mutex lock implementation.
    - **'mutex_lock/'** has the code, build system and scripts for the atomic operations implementation.
- internal structure of root folders

# Build and Run

To built the 