First run of the `results.py` script on Christmas Eve of 2024.

The functions for the parallel code used at this point are the following:

**Rows method**

```C
int back_substitution_by_row_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
) {

    size_t row, column;
    double local_var = 0.0;
    unsigned long internal_iterations;

    for(row = n - 1; row < n; row--) {
        local_var = b[row];
        internal_iterations = row + 1 - n;

        if(internal_iterations > threadcount) {
#pragma omp parallel for num_threads(threadcount) default(none)                \
    reduction(- : local_var) private(column) shared(A, row, x, n)              \
    schedule(runtime)
            for(column = row + 1; column < n; column++) {
                local_var -= A[row][column] * x[column];
            }
        } else {
#pragma omp parallel for num_threads(internal_iterations) default(none)        \
    reduction(- : local_var) private(column) shared(A, row, x, n)              \
    schedule(runtime)
            for(column = row + 1; column < n; column++) {
                local_var -= A[row][column] * x[column];
            }
        }

        x[row] = local_var / A[row][row];
    }

    return 0;
}
```

**Columns method**

```C
int back_substitution_by_column_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
) {

    long long row, column;

#pragma omp parallel num_threads(threadcount)
    {

#pragma omp for schedule(runtime)
        for(row = 0; row < (long long)n; row++)
            x[row] = b[row];

#pragma omp for schedule(runtime)
        for(column = (long long)n - 1; column >= 0; column--) {
            x[column] /= A[column][column];
            for(row = 0; row < column; row++)
                x[row] -= A[row][column] * x[column];
        }
    }
    return 0;
}
```