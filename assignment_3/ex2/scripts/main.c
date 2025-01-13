#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////
// Local includes
///////////////////////////////

#include "linalgebra.h"

////////////////////////////////
// Local defines
///////////////////////////////

////////////////////////////////
// Global Variables
///////////////////////////////

////////////////////////////////
// Function Declerations
///////////////////////////////

////////////////////////////////
// Function Definitions
///////////////////////////////

int main(int argc, char *argv[]) {

    /***********************************
     *  Local variables
     ***********************************/

    size_t n; // number of rows and columns of matrix

    /***********************************
     *  Arguments check
     ***********************************/

    if(argc != 2) {
        printf("Usage: ./app <size>\n");
        return 1;
    }

    n = (int)strtoul(argv[2], NULL, 10);

    /***********************************
     *  Initializations & allocations
     ***********************************/

    /***********************************
     *  Execution of the program
     ***********************************/

    /***********************************
     *  Deallocations and termination
     ***********************************/

    return 0;
}

////////////////////////////////
// Function Definitions
///////////////////////////////