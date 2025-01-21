# Topic Description

This exercise studies a data parallel version of a matrix - vector multiplication.

$$A \cdot x = y$$

From a sequential program a data-parallel version is created, by partition of the matrix $A$ by columns,
creating sub-matrices and sub-vectors, assigned for computation in multiple processes.
In the end all the local result vectors are added up by a summation reduction operation to a final result vector.

This project uses the [mpich](https://www.mpich.org/) MPI implementation to run the parallel program in a big set-up MPI system.

# Folder Structure

internal structure of root folders:
- **[scripts](./scripts/)** : python scripts that automate various processes
- **[source](./source/)** : has all the C source code, which is the [sequential](./source/sequential/) and the [parallel](./source/parallel/) code implementation.
Each of these project has the following sub-folders:
   - **src** : has all the C source code, along the `main.c` function
   - **inc** : all the included header files for the C source code
   - **build** : the folder will be created if you build the project. Inside it there is the executable named 'app'
- **[results](./results/)** : the folder than contains results of various runs

Inside every folder there is a dedicated readme for more informations.

# Build and Run

The build and run process can be performed with scripts in the [scripts](./scripts/) directory
and it is advised to do so.
However the build system used is make and you can build and run manually with the following commands.

## Build with make

First go to the source folders where the `Makefile`s reside

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
