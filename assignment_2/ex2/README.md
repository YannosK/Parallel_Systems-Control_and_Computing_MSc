# Exercise Implementation

In this problem, the user must input the number of threads and a number of iterations

_Problem description_

The C programs of this exercise take as arguments the number if threads and number of iterations.
After they execute they print to the terminal the expected value of the common variable after processing and its actual value.
They also computes the execution time.

Python scripts are used to automate the build process and
the execution for various thread counts and iterations.
They also check that the expected and the actual value of the common variable is the same, for all the executions,
and they gather the timing results and the computed efficiencies in .csv files

# Folder Structure

internal structure of root folders:
- **[scripts](./src/)** : python scripts that automate various processes
- **[src](./src/)** : has all the C source code, which is the main code of the implementations
- **[inc](./inc/)** : all the included header files for the C source code
- **[build](./build/)** : the folder will be created if you build the project. Inside it there is the executable named 'app'
- **[results](./results/)** : the folder than contains results of various runs

Inside every folder there is a dedicated readme for more informations.

# Build and Run

The build and run process can be performed with scripts in the [scripts](./scripts/) directory
and it is advised to do so.
However the build system used is make and you can build and run manually with the following commands.

## Build with make

To clean all the produced binaries run
```
make clean
```

To build the executable, which will be stored inside the 'build/' folder run
```
make
```

To run the executable go to the 'build/' folder and run it.
See the needed arguments and add them accordingly,
based on the usage message on the console.
