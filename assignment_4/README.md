# Topic Description

In this assignment the goal is to accelerate an n-body gravitational interaction system simulator on an NVIDIA GPU device, using the CUDA API. The [`reference_code`](./reference_code.cu) was  provided from the "Getting started with Accelerated Computing in CUDA C/C++" NVIDIA course and by refactoring the goal was to achieve acceleration of the application. More specifically the simulator should run in less than 0.9 seconds for for 4096 bodies and less than 1.3 seconds for 65536 bodies.

There are two (albeit very similar) approaches to the problem, that each one of us came up with. The one is in [approach-1](./approach-1/) and the other in [approach-2](./approach-2/). The report files where generated with the `nsys` profiler and can be used with the Nsight System graphical profiler from the CUDA toolkit.


# Compile and run

To test the applications:

```
make clean
make all
make run
```