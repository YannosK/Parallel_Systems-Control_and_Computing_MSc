# Topic Description

This exercise studies the parallelization of two back-substitution algorithms to solve linear systems of equations, that are represented by upper-triangular matrices, in the form:

$$A \cdot x = b$$

$A$ is the upper triangular coefficient matrix, $x$ is a vector of the unknown variables of the equations and $b$ is the zero-order coefficient vector.

The algorithms tested are back-substitution by row and back-substitution by column. They are written in the C Programming Language. For their parallel version OpenMP library is used.

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
