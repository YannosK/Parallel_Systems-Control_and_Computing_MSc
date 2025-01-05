# C Header Files Folder

This folder contains all the local custom header files that the main program uses. These are linked and compiled automatically in the build process.

The [`equations_system.h`](./equations_system.h) header contains the funtion declarations for the equations system custom library, that contains the functions that substitute the sequential and parallel versions of the back-substitution alforithms, that are tested in this exercise. For more informations see the function contracts.

The [`timer.h`](./timer.h) header contains a `define` to get time snapshots and it is the main mechanism for timing measurements.
