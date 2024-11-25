In this problem, the user must input the number if threads and a number of iterations

An array of integers with the size of the thread number is created. Each thread updates a single element of the array, by adding 1, as many times as the inputted iterations number. This creates false sharing between the processing cores.

The folder 'initial/' has the code and the executable of this problem. The folders 'solution_1' 'solution_2' have two solution to battle false sharing.

Solution 1 is updating each element and write it to the matrix in the end. This way there is race condition only in the final updates. Solution 2 is spacing out the elements inside the matrix by adding junk. In this latter case the cache line size of the system is needed. The python scripts get that automatically