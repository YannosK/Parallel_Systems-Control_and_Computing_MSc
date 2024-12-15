#include <stdio.h>
#include <stdlib.h>

/*
 * Parse input from file and store it in an array.
 * It is assumed that the input file contains a matrix of integers
 * separated by semicolons.
 * Also, the array should be able to store all the values of the
 * input file as there is no check for overflows.
 *
 * Parameters:
 * - filename: the name of the file to read from.
 * - arr: the array to store the values in.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int parse_input_from_file(char *filename, int *arr) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }

    int index = 0;
    int cell;

    while(fscanf(file, "%d;", &cell) != EOF) {
        *(arr + index++) = cell;
    }

    return 0;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
    for(int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("\n");
#endif

    int arr[10][10];
    char *filename = "test/test_10.txt";

    parse_input_from_file(filename, &arr[0][0]);

#ifdef DEBUG
    printf("Matrix read from file:\n");
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++)
            printf("%d ", arr[i][j]);
        printf("\n");
    }
#endif

    return 0;
}